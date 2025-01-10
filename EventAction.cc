﻿//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B3/B3a/src/EventAction.cc
/// \brief Implementation of the B3a::EventAction class

#include "EventAction.hh"

#include "RunAction.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4THitsMap.hh"
// #include "G4UnitsTable.hh"
#include <fstream>  // Thêm thư viện để ghi file
#include <sstream>  // Để xử lý chuỗi cho tên file

namespace B3a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*evt*/) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{
  // Hits collections
  //
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if (!HCE) return;

  // Get hits collections IDs
  if (fCollID_cryst < 0) {
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();
    fCollID_cryst = SDMan->GetCollectionID("detector/edep");
    fCollID_patient = SDMan->GetCollectionID("patient/dose");
  }

  // Energy in crystals : identify 'good events'
  //
  const G4double eThreshold = 700 * keV;
  G4int nbOfFired = 0;

  auto evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_cryst));

  for (auto& mapElement : (*evtMap->GetMap())) {
    auto edep = *(mapElement.second);
    if (edep > eThreshold) ++nbOfFired;
    // auto copyNb  = mapElement.first;
    // G4cout << "\n  cryst" << copyNb << ": " << edep/keV << " keV ";
  }
  if (nbOfFired == 2) fRunAction->CountEvent();

  // Dose deposit in patient
  //
  G4double dose = 0.;

  evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_patient));

  for (auto& mapElement : (*evtMap->GetMap())) {
    dose += *(mapElement.second);
    // auto copyNb  = mapElement.first;
    // G4cout << "\n  patient" << copyNb << ": " << G4BestUnit(dose,"Dose");
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

      // Tạo một file log riêng cho verbose
      std::ofstream verboseFile("verbose_output_event_" + std::to_string(eventID) + ".txt");

      if (verboseFile.is_open()) {
          // Chuyển hướng đầu ra của verbose vào file log này
          G4UImanager* uiManager = G4UImanager::GetUIpointer();
          std::streambuf* originalCoutBuf = std::cout.rdbuf();  // Lưu lại bộ đệm stdout
          std::cout.rdbuf(verboseFile.rdbuf());  // Chuyển hướng G4cout vào file log

          // Lệnh verbose
          uiManager->ApplyCommand("/control/verbose 2");  // Đặt mức độ verbose cho control
          uiManager->ApplyCommand("/tracking/verbose 2"); // Đặt mức độ verbose cho tracking

          // Đảm bảo thông tin được ghi vào file
          std::cout.rdbuf(originalCoutBuf);  // Quay lại stdout mặc định
          verboseFile.close();  // Đóng file verbose output
      }

  }
  else {
      G4cerr << "Không thể mở file: " << filename.str() << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3a
