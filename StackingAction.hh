#ifndef B3StackingAction_h
#define B3StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"

class G4Track;

namespace B3
{

    /// Stacking action class : quản lý các particle mới sinh ra
    ///
    /// Lý do không theo dõi neutrino thứ cấp là vì chúng bị giết ngay lập tức
    /// trước khi các particle tạo ra được đưa vào stack.

    class StackingAction : public G4UserStackingAction
    {
    public:
        StackingAction() = default;
        ~StackingAction() override = default;

        G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override;
        void PostUserTrackingAction(const G4Track* track);  // Không cần override
    };

}  // namespace B3

#endif
