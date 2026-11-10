# NarrativeEngine HoH - Prompt Overrides 0.8.0

Replaces three of **NarrativeEngine v0.8.0**'s SkyrimNet prompts by path (the higher-priority MO2 mod wins the file;
SkyrimNet reads the winner). No DLL, no ESP, no save changes. Successor of `[NoDelete]NarrativeEngine HoH - Prompt
Overrides` (0.7.x) — same ideas, rebuilt on the 0.8.0 files, with `hold` removed.

`SKSE/Plugins/SkyrimNet/external/pdusen.narrative-engine/prompts/`

| File | Base | Changes |
| ---- | ---- | ------- |
| `narrative_engine_action_select.prompt` (Director picks the event) | v0.8.0 file (keeps upstream's v0.7.3 rule about picking only offered senders/memories/groups) | **Magnitude is an upper bound** — the smallest move that serves the moment. **Choosing a reason (v3):** look for what changed since the person last spoke with the player (news, consequence, request, offer, discovery) rather than re-arguing an old quarrel; spread events across people; after a confrontation, deal, fight or intimate scene choose the smallest action offered. **What moving tension means:** stakes, news, offers, discoveries, requests, warnings; a grievance becomes a demand, a hard bargain, a warning or news of a decision — intimidation only for someone who means to act against the player. **Letters** suit news, gratitude, reactions. **A visit must be worth the journey**; among visitors, the newest reason wins. Letter hints appear only in the letter section. |
| `narrative_engine_letter_compose.prompt` | the 0.7.x HoH override (upstream file unchanged since v0.7.1) | Cancel when there is no fresh, grounded reason; **tone rewritten 2026-10-27**: feelings in the writer's own voice, a grievance usually `urgent`, intimidation only for a writer who means to act against the player; length ~100-140 words, limit `{{ max_words }}` (260 with the Settings 0.8.0 mod). |
| `narrative_engine_visit_compose.prompt` | the 0.7.x HoH override (upstream file unchanged since v0.7.1) | Same cancel and (rewritten) tone rules; narration no longer claims a journey (visitors are placed nearby). |

### Test results (2026-10-27, real 0.8.0 contexts, your NanoGPT models)

Full report: `D:\ProgramMems\SkyrimNet-NarrativeEngine\tools\AB080_REPORT.md`.

- Unit tests: 142 checks on 18 real contexts, 0 failures. Live: 205 calls, 0 invalid picks / parse errors / word-count
  breaks / `hold`.
- Composer tone: `menacing` 58% with upstream, 84% with the old HoH rule, **8%** with the rewritten rule (now mostly
  `urgent`). Letters at the 260 limit stay ~115-147 words instead of 169-231.
- Director picks (first HoH version, gemma-4-31b): no measurable change from upstream. The v3 version below lowers confrontational reasoning on every model tested. Repeats like Mikael
  re-using his visit's memory need the code fix (shared letter/visit memory record), not prompt text.
- The cancel rule never fired in these tests (0/104), so expect "not now" to come mostly from the 15-minute gap.

### Director model (tested 2026-10-27, same 7 real cases, HoH prompt)

| Director | Invalid picks | Who was picked (21) | Confrontational wording | Ambush when offered | Median time |
| -------- | ------------- | ------------------- | ----------------------- | ------------------- | ----------- |
| google/gemma-4-31b-it (current) | 0 | Vesyra 8, Lyne 6, Mikael 3 | 13/21 | 4/6 | 3.5 s (spikes 21 s) |
| **deepseek/deepseek-v4.1-flash** | 0 | Vesyra 15, **Mikael 3, Adrianne 2, Coralyn 1** | 6/21 | 0/6 | 2.6 s |
| deepseek/deepseek-v4.1-flash-1017 | 0 | Vesyra 19, Lyne 1 | 6/21 | 1/6 | 2.6 s |
| z-ai/glm-5.3-flash (reasoning low) | 0 | Vesyra 15, Lyne 2 | 3/21 | 4/6 | 3.5 s |

Recommended: **deepseek-v4.1-flash** for `narrative_engine_director` (SkyrimNet dashboard → AI → Models), the same
model as the composer. It was the only Director that spread events across several people with new reasons (Adrianne
with business news, Coralyn, Mikael). "Confrontational wording" only counts words in the Director's note: GLM's low
score was the same Vesyra quarrel described more gently, not different events. Trade-off: DeepSeek never chose an
ambush in this test; if you miss them, use gemma-4-31b. (If you try GLM: reasoning must be `low`, not off.)

The Director prompt was revised after this test: the "letter" hints now live only in the letter section (DeepSeek
had picked a letter when only a visit was offered), "fresh reason" means something new since they last spoke, and
grievances become demands, bargains or warnings rather than confrontations.

### Why `hold` is gone

Since v0.7.3 an answer that names no offered action (such as `hold`) is retried once and then fails like a failed LLM
call: two Director calls, and the next tick tries again. The cheap "not now" is now the writers' **cancel** (an empty
letter/briefing): the event counts as fired, the global gap (`iBeatCooldownSeconds`, 900 s with the Settings mod) starts
again, and nothing reaches the player. It costs one Director call plus one writer call. A cancelled person is not put on
cooldown, so the "Choosing a reason" rule is what keeps the Director from picking them again.

Cancels show in the log as `body word count 0` / `briefing word count 0` followed by `COMPOSE failed` — expected.

### Deliberately not in this mod

`narrative_engine_story_eval.prompt` (the tension score): scoring intimate scenes lower would change what the new
threshold 65 reacts to. It is left for a later, separate change so the next log comparison can tell which change did
what.

## Install (Skyrim closed)

1. Enable this mod and place it **below** `[NoDelete]NarrativeEngine-v0.8.0` in MO2's left pane (higher priority).
   Order relative to `NarrativeEngine HoH - Settings 0.8.0` does not matter (no shared files).
2. Keep the old `[NoDelete]NarrativeEngine HoH - Prompt Overrides` **disabled** — its `action_select` still has `hold`.

Validation: section markers, no code fences, no `hold`, v0.7.3 sentence present, diff against v0.8.0 checked. Not yet
rendered through SkyrimNet (game was not running) — watch the first session's log for render errors.

## Rollback

Disable the mod; v0.8.0's own prompts are used again.
