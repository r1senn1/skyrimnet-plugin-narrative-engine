# NarrativeEngine HoH - Settings 0.8.0

Settings-only MO2 mod for **NarrativeEngine v0.8.0**: one file, `SKSE/Plugins/NarrativeEngine.ini`. It is the
v0.8.0 file unchanged except the values listed below, each marked with a `; HoH:` comment. Everything else (including the new
`[Plots]` section) stays at 0.8.0 defaults. No DLL, no ESP, no prompts; safe on an existing save.

| Key | 0.8.0 | HoH | Why (from two clean 0.8.0 sessions, 2026-10-26/27) |
| --- | ----- | --- | -------------------------------------------------- |
| `[Director] iAdvanceThresholdRisingAction` | 80 | **65** | In 96 evaluations, RisingAction tension averaged ~21 and peaked at 65 twice (once NarrativeEngine's own ambush, once an intimate scene); it never reached 80, so the story sat in RisingAction for 3.8 h and every event asked for a bigger raise. Note: this lets the cycle move, it does not add calm events: Climax and FallingAction (the only "lower" phases) are left at the next check because scores fall to 10-20 at once, so events stay mostly "raise". |
| `[BeatSystem] iBeatCooldownSeconds` | 120 | **900** | Twice a visit came 7 minutes after an ambush. Counted in active play only (not paused, combat or dialogue), from the end of the previous event. 15 minutes removes the back-to-back pairs; normal pace (one event per ~30-45 min, limited by available NPCs) stays the same. |
| `[Beats] iLetterContentMaxWords` | 150 | **260** | Earlier, letters of 151-223 words were discarded whole. |
| `[Beats] sBlacklistedSenders` | empty | **`Delphine3DNPC; Sven3DNPC; Delphine; Sven`** | EditorIDs only match with an EditorID-fix DLL (none for 0.8.0 yet), so the display names are added. |

**Context sizes (added 2026-10-27)** — more history for the Director and the writers, so they can see who already
wrote or visited and what was already said:

| Key | 0.8.0 | HoH |
| --- | ----- | --- |
| `iActionSelectEventRenderCap` | 10 | **30** (10 events covered only ~24 min of chat) |
| `iActionSelectLetterMemoryRenderCap` / `iActionSelectVisitMemoryRenderCap` | 6 | **10** |
| `iLetterComposeMemoryRenderCap` / `iVisitComposeMemoryRenderCap` | 6 | **10** |
| `iLetterComposeDialogueRenderCap` / `iVisitComposeDialogueRenderCap` | 25 | **50** |
| `iVisitConclusionPollRecentLinesRenderCap` | 8 | **17** |

Cost: somewhat longer prompts (more input tokens per call).

Replaces `[NoDelete]NarrativeEngine HoH - Settings` (built for 0.7.x, with more changes that the data did not
justify).

## Install (Skyrim closed)

1. Enable this mod and place it **below** `[NoDelete]NarrativeEngine-v0.8.0` (higher priority) so its INI wins.
2. Keep the old `[NoDelete]NarrativeEngine HoH - Settings` **disabled**.
3. `overwrite\MCM\Settings\NarrativeEngine.ini` (dashboard/MCM changes) still overrides this file. Today it only holds
   the Shift requirement for the dashboard hotkey. Anything you change in the dashboard's Settings tab lands there.

## Tuning

Too quiet: `iBeatCooldownSeconds=600`. Too busy: raise it. Story still stuck in RisingAction: lower the threshold
to 60.

## Rollback

Disable the mod; NarrativeEngine falls back to its own 0.8.0 INI.
