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

## Kehityksen tila

**Vaihe 1 (kesken)** — Perusluokat olemassa, kääntyvät, mutta eivät vielä muodosta pelattavaa kokonaisuutta.

Seuraavat askeleet:
- [ ] GridX/GridY-synkronointi liikkeen yhteydessä
- [ ] MovementComponent-GridManager-yhteys (validointi)
- [ ] ProcessNextRegister() — korttien kytkentä robottien liikkeisiin
- [ ] ARobotController (pelaajan syötteet)
- [ ] HealthComponent (vahinko ja elämät)
- [ ] Korttien jakelu- ja käsijärjestelmä
- [ ] UI: ohjelmointinäkymä, HUD, korttislotit
- [ ] Kenttävaarat: laserit, liukuhihnat, kuopat toiminnallisuutena
- [ ] Checkpoint-keräysjärjestelmä
- [ ] Visuaalinen debug-piirto ruudukolle

## Lisenssi

Copyright (c) 2026 Robot Rally Team. All Rights Reserved.
