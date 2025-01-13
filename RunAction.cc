#include "RunAction.hh"

#include "PrimaryGeneratorAction.hh"

#include "G4AccumulableManager.hh"
#include "G4ParticleGun.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

using namespace B3;

namespace B3a
{

	RunAction::RunAction()
	{
		// add new units for dose
		//
		const G4double milligray = 1.e-3 * gray;
		const G4double microgray = 1.e-6 * gray;
		const G4double nanogray = 1.e-9 * gray;
		const G4double picogray = 1.e-12 * gray;

		new G4UnitDefinition("milligray", "milliGy", "Dose", milligray);
		new G4UnitDefinition("microgray", "microGy", "Dose", microgray);
		new G4UnitDefinition("nanogray", "nanoGy", "Dose", nanogray);
		new G4UnitDefinition("picogray", "picoGy", "Dose", picogray);

		// Register accumulable to the accumulable manager
		G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
		accumulableManager->RegisterAccumulable(fGoodEvents);
		accumulableManager->RegisterAccumulable(fSumDose);
	}

	void RunAction::BeginOfRunAction(const G4Run* run)
	{
		// reset accumulables to their initial values
		G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
		accumulableManager->Reset();

		// inform the runManager to save random number seed
		G4RunManager::GetRunManager()->SetRandomNumberStore(false);
	}

	void RunAction::EndOfRunAction(const G4Run* run)
	{
		G4int nofEvents = run->GetNumberOfEvent();
		if (nofEvents == 0) return;

		// Merge accumulables
		G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
		accumulableManager->Merge();
	}

}  // namespace B3a
