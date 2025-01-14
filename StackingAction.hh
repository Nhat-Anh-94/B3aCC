// StackingAction.hh

#ifndef B3StackingAction_h
#define B3StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"
#include <fstream> // ?? s? d?ng std::ofstream

class G4Track;

namespace B3
{

    /// Stacking action class : manage the newly generated particles
    ///
    /// One wishes do not track secondary neutrino. Therefore one kills it
    /// immediately, before created particles will be put in a stack.

    class StackingAction : public G4UserStackingAction
    {
    public:
        StackingAction() = default;
        ~StackingAction() override = default;

        G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override;

        // Khai báo ph??ng th?c PostUserTrackingAction ?? ghi thông tin tracking vào file
        void PostUserTrackingAction(const G4Track* track);

    private:
        std::ofstream outFile;  // ??i t??ng file ?? ghi thông tin tracking
    };

}  // namespace B3

#endif
