# UMG HUD Migration Plan

**Status:** In Progress
**Priority:** Medium
**Complexity:** Medium

## Tilanne

HUD-järjestelmässä on kaksi rinnakkaista toteutusta:

1. **Canvas HUD** (`DrawHUD()`) — vanha, toimiva, C++-pohjainen piirto
2. **UMG Widget** (`URobotRallyMainWidget`) — uusi, Blueprint-laajennettava, ei vielä käytössä

`bUseUMGWidgets = true` esti Canvas HUD:n, mutta `MainWidgetClass` oli null → kumpaakaan ei näytetty.

**Väliaikainen korjaus:** `bUseUMGWidgets = false` (commit: fix Canvas HUD visibility)

---

## Tavoite

Korvata Canvas HUD täysin UMG-widgeteillä. Lopputulos:

- `WBP_MainHUD` näyttää terveyden, elämiä, checkpointit, pelitilan
- `WBP_ProgrammingDeck` näyttää käsikortteja ja rekisterejä
- `WBP_CardSlot` yksittäinen korttiwidget (jo tehty)
- Canvas HUD poistetaan kun UMG on valmis

---

## Toteutusaskeleet

### 1. Luo WBP_MainHUD Blueprint
- [ ] Luo editorissa uusi Widget Blueprint pohjana `URobotRallyMainWidget`
- [ ] Tallenna: `Content/UI/WBP_MainHUD`
- [ ] Lisää vaaditut komponentit (BindWidget-muuttujat):
  - `ProgrammingDeck` (UProgrammingDeckWidget)
  - `EventLogBox` (UVerticalBox)

### 2. Luo WBP_ProgrammingDeck Blueprint
- [ ] Luo Widget Blueprint pohjana `UProgrammingDeckWidget`
- [ ] Tallenna: `Content/UI/WBP_ProgrammingDeck`
- [ ] Lisää korttirivit käyttäen `WBP_CardSlot`-widgetiä

### 3. Aseta MainWidgetClass HUD:lle
- [ ] Avaa editorissa `ARobotRallyHUD`-pohjainen Blueprint (tai luo `BP_RobotRallyHUD`)
- [ ] Aseta `MainWidgetClass` → `WBP_MainHUD`
- [ ] Aseta GameModessa `HUDClass` → `BP_RobotRallyHUD`

### 4. Ota UMG käyttöön
- [ ] Vaihda `bUseUMGWidgets = false` → `true` (RobotRallyHUD.h)
- [ ] Testaa standalone: terveys, elämiä, kortit näkyvät
- [ ] Testaa multiplayer: replication toimii

### 5. Poista Canvas HUD
- [ ] Poista `DrawCardSelection()` ja muu Canvas-koodi
- [ ] Poista `bShowCanvasHUD`-lippu
- [ ] Poista `bUseUMGWidgets`-lippu (UMG on ainoa tapa)

---

## Tiedostot

| Tiedosto | Muutos |
|---|---|
| `Source/RobotRally/RobotRallyHUD.h` | `bUseUMGWidgets` oletusarvo |
| `Source/RobotRally/RobotRallyHUD.cpp` | Canvas-koodin poisto (vaihe 5) |
| `Content/UI/WBP_MainHUD` | Uusi Blueprint (vaihe 1) |
| `Content/UI/WBP_ProgrammingDeck` | Uusi Blueprint (vaihe 2) |

---

## Huomioita

- `URobotRallyMainWidget` käyttää `meta = (BindWidget)` — Blueprint-widgeteissä **täytyy** olla samanniminen komponentti tai compile-virhe tulee
- `ProgrammingDeckWidget` ja `EventLogBox` ovat pakollisia BindWidget-kenttiä
- Canvas HUD jätetään toistaiseksi fallbackiksi (`bUseUMGWidgets = false`)
