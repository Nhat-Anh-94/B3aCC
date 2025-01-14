// StackingAction.cc

#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4NeutrinoE.hh"
#include <fstream>

namespace B3
{

    // Constructor mở file ghi thông tin tracking
    StackingAction::StackingAction()
    {
        outFile.open("track_info_verbose.txt", std::ios::app);  // Mở file ở chế độ append
        if (!outFile.is_open()) {
            G4cerr << "Error: Unable to open file for writing!" << G4endl;
        }
    }

    // Destructor đóng file khi không còn sử dụng
    StackingAction::~StackingAction()
    {
        if (outFile.is_open()) {
            outFile.close();
        }
    }

    // Phương thức phân loại track
    G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
    {
        // Keep primary particle
        if (track->GetParentID() == 0) return fUrgent;

        // Kill secondary neutrino
        if (track->GetDefinition() == G4NeutrinoE::NeutrinoE())
            return fKill;
        else
            return fUrgent;
    }

    // Phương thức ghi thông tin mỗi khi một tracking event xảy ra
    void StackingAction::PostUserTrackingAction(const G4Track* track)
    {
        if (track) {
            // Thu thập thông tin cần thiết từ track và step
            G4int trackID = track->GetTrackID();  // ID của track
            G4String particleName = track->GetDefinition()->GetParticleName();  // Tên của hạt
            G4double energy = track->GetKineticEnergy() / keV;  // Năng lượng của hạt (đơn vị keV)

            // Lấy bước hiện tại của track
            G4Step* step = track->GetStep();
            G4ThreeVector position = step->GetPostStepPoint()->GetPosition();  // Vị trí cuối của bước
            G4double x = position.x() / mm;  // Vị trí X
            G4double y = position.y() / mm;  // Vị trí Y
            G4double z = position.z() / mm;  // Vị trí Z

            // Lấy thông tin từ bước (Step)
            G4double dEStep = step->GetTotalEnergyDeposit() / keV;  // Năng lượng mất đi trong bước (keV)
            G4double stepLength = step->GetStepLength() / cm;  // Chiều dài bước (cm)
            G4double trackLength = track->GetTrackLength() / cm;  // Chiều dài của track (cm)

            // Lấy thông tin volume
            G4String volumeName = "None";  // Nếu hạt chưa vào volume, gán tên là "None"
            if (step->GetPostStepPoint()->GetPhysicalVolume()) {
                volumeName = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();  // Tên volume hạt đang di chuyển qua
            }

            // Lấy quá trình (process) mà hạt đang trải qua
            G4String processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

            // Ghi thông tin vào file theo định dạng giống verbose 2
            outFile << "Step# " << step->GetStepNumber()
                << "   " << x << " mm   " << y << " mm   " << z << " mm   "
                << energy << " keV   " << dEStep << " keV   "
                << stepLength << " cm   " << trackLength << " cm   "
                << volumeName << "   " << processName
                << std::endl;
        }
    }

}  // namespace B3
