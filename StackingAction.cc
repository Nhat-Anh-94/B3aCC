#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4NeutrinoE.hh"
#include "G4SystemOfUnits.hh"
#include "G4VProcess.hh"
#include <fstream>  // Thư viện cho việc ghi file

namespace B3
{

    // Hàm phân loại track mới
    G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
    {
        // Giữ particle gốc
        if (track->GetParentID() == 0) return fUrgent;

        // Giết neutrino thứ cấp (nếu có)
        if (track->GetDefinition() == G4NeutrinoE::NeutrinoE())
            return fKill;

        // Các particle khác vẫn được xử lý bình thường
        return fUrgent;
    }

    // Hàm ghi thông tin của mỗi bước vào file
    void StackingAction::PostUserTrackingAction(const G4Track* track)
    {
        // Lấy bước hiện tại
        const G4Step* step = track->GetStep();

        // Lấy các thông tin cần thiết
        G4String processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
        G4ThreeVector position = step->GetPostStepPoint()->GetPosition();
        G4double energy = track->GetKineticEnergy();

        // Ghi thông tin vào file
        std::ofstream outFile("tracking_info.txt", std::ios_base::app);  // Mở file ở chế độ append
        if (outFile.is_open()) {
            outFile << "Step# " << step->GetTrack()->GetCurrentStepNumber()  // Số bước hiện tại
                << ", Position: " << position
                << ", Energy: " << energy / keV << " keV"
                << ", Process: " << processName << std::endl;
            outFile.close();  // Đảm bảo đóng file sau khi ghi
        }
        else {
            G4cout << "Error opening file for writing!" << G4endl;  // In ra lỗi nếu không mở được file
        }
    }

}  // namespace B3
