# ESPRazorBlade Agent Runbook

Quick operational notes for AI agents working in this repository.

## Build Defaults

- Board FQBN: `esp32:esp32:esp32c3`
- Main sketch: repo root (`ESPRazorBlade.ino`)
- Primary example sketch: `examples/Basic_Usage`
- Build tool: `arduino-cli`

## Compile Commands

- Preferred:
  - `./scripts/deploy.sh --dev`
- If `.arduino-build` permission issues occur:
  - `arduino-cli compile --fqbn "esp32:esp32:esp32c3" --build-path "/tmp/esprazorblade-basic-usage-build" --library "<repo_root>" "<repo_root>/examples/Basic_Usage"`
- Main sketch compile:
  - `arduino-cli compile --fqbn "esp32:esp32:esp32c3" --build-path "/tmp/esprazorblade-main-build" "<repo_root>"`

## Deploy Workflow

- Start with:
  - `./scripts/deploy.sh --help`
- Use only options supported by the current branch version of `scripts/deploy.sh`.

## Reporting Expectations

When reporting compile/deploy results, include:

- Success or failure
- Flash usage
- RAM usage
- Blocking error lines (if any)
