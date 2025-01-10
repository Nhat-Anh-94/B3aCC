#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <sstream>

namespace B3a
{
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void EventAction::EndOfEventAction(const G4Event* evt)
    {
        // Hits collections
        G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
        if (!HCE) return;

        // Get hits collections IDs
        if (fCollID_cryst < 0) {
            G4SDManager* SDMan = G4SDManager::GetSDMpointer();
            fCollID_cryst = SDMan->GetCollectionID("detector/edep");
            fCollID_patient = SDMan->GetCollectionID("patient/dose");
        }

        // Energy in crystals : identify 'good events'
        const G4double eThreshold = 700 * keV;
        G4int nbOfFired = 0;
        auto evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_cryst));

        for (auto& mapElement : (*evtMap->GetMap())) {
            auto edep = *(mapElement.second);
            if (edep > eThreshold) ++nbOfFired;
        }
        if (nbOfFired == 2) fRunAction->CountEvent();

        // Dose deposit in patient
        G4double dose = 0.;
        evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_patient));

        for (auto& mapElement : (*evtMap->GetMap())) {
            dose += *(mapElement.second);
        }
        if (dose > 0.) fRunAction->SumDose(dose);

        // Tạo tên file và ghi vào file riêng biệt cho mỗi sự kiện
        G4int eventID = evt->GetEventID();
        std::stringstream filename;
        filename << "event_" << eventID << "_log.txt";  // Tên file: event_<eventID>_log.txt

        std::ofstream outFile(filename.str());
        if (outFile.is_open()) {
            // Ghi thông tin sự kiện vào file
            outFile << "Sự kiện ID: " << eventID << "\n";
            outFile << "Số lượng hạt phát ra: " << nbOfFired << "\n";
            outFile << "Liều lượng tại bệnh nhân: " << dose / gray << " Gy\n";

            // Lấy thông tin verbose từ hệ thống Geant4
            G4UImanager* uiManager = G4UImanager::GetUIpointer();

            // Tạo stream để chuyển hướng G4cout và G4cerr vào file
            std::streambuf* originalCoutBuf = std::cout.rdbuf();
            std::ofstream verboseFile("verbose_output_event_" + std::to_string(eventID) + ".txt");
            std::cout.rdbuf(verboseFile.rdbuf());  // Chuyển hướng G4cout vào file

            // Ghi thông tin verbose
            uiManager->ApplyCommand("/control/verbose 2");  // Đặt mức độ verbose cho control
            uiManager->ApplyCommand("/tracking/verbose 2"); // Đặt mức độ verbose cho tracking

            // Ghi chi tiết lệnh verbose vào file
            outFile << "Thông tin từ /control/verbose 2:\n";
            outFile << "   (Đầu ra sẽ được ghi vào file log này)\n";

            outFile << "Thông tin từ /tracking/verbose 2:\n";
            outFile << "   (Đầu ra sẽ được ghi vào file log này)\n";

            // Đảm bảo file được đóng lại sau khi ghi
            std::cout.rdbuf(originalCoutBuf);  // Quay lại stdout mặc định
            verboseFile.close();  // Đóng file verbose output
        }
        else {
            G4cerr << "Không thể mở file: " << filename.str() << G4endl;
        }
    }

}  // namespace B3a
