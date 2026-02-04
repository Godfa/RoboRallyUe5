# RobotRally (UE5)

Digitaalinen toteutus Robot Rally -lautapelistä Unreal Engine 5.7:lla. Pelaajat ohjelmoivat robotteja liikekorteilla ja katsovat niiden suorittavan käskyt ruudukkopohjaisella pelilaudalla.

## Vaatimukset

- Unreal Engine 5.7
- Visual Studio 2022 (MSVC 14.38 tai 14.44)
- Windows 10/11 SDK (22621)

## Projektin avaaminen

1. Kloonaa repo kansioon `H:\Repos\RoboRallyUe5` (tai haluamaasi sijaintiin)
2. Tuplaklikkaa `RobotRally.uproject` avataksesi projektin UE5-editorissa
3. Editori kääntää C++-moduulin automaattisesti

## Projektirakenne

```
Source/RobotRally/
  RobotRally.h/.cpp              Moduulin rekisteröinti
  RobotPawn.h/.cpp               Robottihahmo (ACharacter)
  RobotMovementComponent.h/.cpp  Ruudukkoliike (interpoloitu)
  GridManager.h/.cpp             Pelilauta (10x10 TMap-ruudukko)
  RobotRallyGameMode.h/.cpp      Pelitilakone (FSM)
```

## Pelimekaniikka

### Pelitilat
1. **Programming** — Pelaaja valitsee 5 liikekorttia rekistereihin
2. **Executing** — Robotit suorittavat käskyt rekisteri kerrallaan
3. **GameOver** — Peli päättyy

### Korttityypit
| Kortti | Toiminto |
|---|---|
| Move1 / Move2 / Move3 | Liiku 1-3 ruutua eteenpäin |
| MoveBack | Liiku 1 ruutu taaksepäin |
| RotateRight | Käänny 90° oikealle |
| RotateLeft | Käänny 90° vasemmalle |
| UTurn | Käänny 180° |

### Ruututyypit
| Tyyppi | Vaikutus |
|---|---|
| Normal | Ei vaikutusta |
| Pit | Robotti tuhoutuu |
| Conveyor (N/S/E/W) | Siirtää robottia vuoron lopussa |
| Laser | Vahingoittaa robottia |
| Checkpoint | Kerättävä järjestyksessä voittaakseen |

## Arkkitehtuuri

```
ARobotRallyGameMode          Pelitilakone, vuorologiikka
  |
  +-- ARobotPawn             Robottihahmo ruudukolla
  |     +-- URobotMovementComponent  Interpoloitu ruudukkoliike
  |
  +-- AGridManager           Pelilauta, tile-data, koordinaattimuunnokset
```

### Ruudukkojärjestelmä
- `TMap<FIntVector, FTileData>` — avain (x, y, 0), arvo sisältää tyypin ja checkpoint-numeron
- `GridToWorld()` / `WorldToGrid()` — muunnokset ruudukko- ja maailmakoordinaattien välillä
- Ruudun koko: 100 UE-yksikköä
- Laudan ulkopuoli = kuoppa (Pit)

### Liikejärjestelmä
- `MoveInGrid(Distance)` — asettaa kohdepisteen forward-vektorin suuntaan
- `RotateInGrid(Steps)` — kääntää kohderotaatiota 90° askelin
- Tick-funktiossa interpoloidaan pehmeästi kohteeseen (`VInterpTo` / `RInterpTo`)

## Toteutussuunnitelma

### Vaihe 0: Projektin luonti & rakenne — VALMIS
- [x] UE5-projekti luotu nimellä RobotRally
- [x] Kansiorakenne standardin mukaisesti
- [x] Plugin-asetukset (Enhanced Input)

### Vaihe 1: Perusrakenne (C++) — KESKEN
- [x] `ARobotPawn` — Header ja CPP, perusrakenne
- [x] `URobotMovementComponent` — `MoveInGrid()` ja `RotateInGrid()` interpoloinnilla
- [x] `AGridManager` — Ruudukon generointi ja tile-tarkistukset
- [ ] GridX/GridY-synkronointi liikkeen yhteydessä
- [ ] MovementComponent-GridManager-yhteys (ruudukkovalidointi: kuopat, rajat, esteet)
- [ ] Visuaalinen debug-piirto ruudukolle

### Vaihe 2: Logiikkakerros (GameMode & kortit)
- [x] `ARobotRallyGameMode` — Tilakone (Programming/Executing/GameOver), 5 rekisteriä
- [x] `FRobotCard` — Kortin tyyppi (ECardAction) ja prioriteetti
- [ ] `ProcessNextRegister()` — Korttien kytkentä robottien liikkeisiin
- [ ] `UCardDataAsset` — Data-driven lähestymistapa korteille
- [ ] Korttien jakelu- ja käsijärjestelmä (satunnaiset kortit pelaajalle)
- [ ] Prioriteettipohjainen suoritusjärjestys robottien välillä

### Vaihe 3: Pelaajan ohjaus
- [ ] `ARobotController` (APlayerController) — Hiiren klikkaukset ja korttisyötteet
- [ ] `HealthComponent` — Vahinko, elämät, robotin tuhoaminen ja respawn
- [ ] Checkpoint-keräysjärjestelmä (järjestyksessä kerättävät liput)

### Vaihe 4: Kenttävaarat & ympäristö
- [ ] Liukuhihnat — Siirtävät robottia vuoron lopussa suunnan mukaan
- [ ] Laserit (`BP_Hazard_Laser`) — Vahingoittavat robottia rekisterin lopussa
- [ ] Kuopat — Robotti tuhoutuu päätyessään kuoppaan
- [ ] Törmäystarkistukset robottien välillä (työntö)

### Vaihe 5: UI & käyttöliittymä
- [ ] `WBP_ProgrammingDeck` — Korttien valintanäkymä (9 korttia kädessä, 5 rekisteriin)
- [ ] `WBP_CardSlot` — Yksittäisen kortin UI-esitys
- [ ] `WBP_HUD` — Terveys, elämät, nykyinen pelivaihe, checkpoint-edistyminen

### Vaihe 6: Sisältö & viimeistely
- [ ] `/Content/RobotRally/Maps/` — Pelitaso(t)
- [ ] `/Content/RobotRally/Blueprints/` — C++-pohjaiset Blueprint-lapsiluokat
- [ ] `/Content/RobotRally/Data/` — DataTablet ja DataAssetit ruudukoille
- [ ] Ääniefektit ja visuaaliset efektit (liike, vahinko, checkpoint)

### Varmistussuunnitelma
- [ ] Automaattiset testit: GridManager-koordinaattimuunnokset, törmäystarkistukset
- [ ] Manuaalinen testaus: korttien valinta, robottien liike, vaarat, checkpoint-keräys

## Lisenssi

Copyright (c) 2026 Robot Rally Team. All Rights Reserved.
