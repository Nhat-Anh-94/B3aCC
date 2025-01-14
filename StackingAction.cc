#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4NeutrinoE.hh"  // Thêm vào để sử dụng G4NeutrinoE
#include "G4SystemOfUnits.hh"  // Thêm vào để sử dụng các đơn vị như keV
#include <fstream>

namespace B3
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
    {
        // keep primary particle
        if (track->GetParentID() == 0) return fUrgent;

        // kill secondary neutrino
        if (track->GetDefinition() == G4NeutrinoE::NeutrinoE())
            return fKill;
        else
            return fUrgent;

        return fUrgent;  // Đảm bảo trả về giá trị mặc định
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void StackingAction::PostUserTrackingAction(const G4Track* track)
    {
        const G4Step* step = track->GetStep(); // Dùng const G4Step* để tránh thay đổi

        // Lấy thông tin cần thiết
        G4String processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
        G4ThreeVector position = step->GetPostStepPoint()->GetPosition();
        G4double energy = track->GetKineticEnergy();

        // Ghi thông tin vào file
        outFile.open("tracking_info.txt", std::ios_base::app);
        if (outFile.is_open()) {
            outFile << "Step# " << step->GetTrack()->GetCurrentStepNumber() // Hoặc thay thế bằng số bước khác
                << ", Position: " << position
                << ", Energy: " << energy / keV << " keV"
                << ", Process: " << processName << std::endl;
            outFile.close();
        }
    }

}  // namespace B3
