# ESTUN ERI SDK Repository Instructions

## Required Reading

For any task involving ERI API usage, integration, debugging, examples, or code
generation, read these sources before changing code:

1. `docs/ERI_SDK_GUIDE.md`
2. `include/ERIParamManager.h`
3. The closest matching file under `examples/`

Do not infer units, return values, connection state, call order, or thread
behavior from an API name. If the sources above do not define a contract,
state that it is unknown and ask for authoritative SDK information.

## Source Priority

When sources differ, use this priority and report the conflict:

1. Public SDK headers for API signatures and documented contracts
2. The example that matches the requested operation
3. `docs/ERI_SDK_GUIDE.md` for cross-cutting workflow guidance
4. `README.md` for build and launch instructions

The binary SDK implementation is not present as source code in this repository.
Do not claim implementation details that cannot be verified from public files.

## Robot Safety

- Never treat example positions, trajectories, IP addresses, ports, tool IDs, or
  user coordinate IDs as production-ready values.
- Before code can move a real robot, require human confirmation of the robot
  model, installation, payload, tool and user frames, units, start pose,
  workspace, collision risk, and site safety boundary.
- Do not run a motion example against physical equipment as an automated test.
- Keep changes surgical and preserve the existing example style.

## Verification

After code changes, run the repository's available lint or static checks and the
relevant build/tests. If no lint or tests exist, say so explicitly and run the
closest non-hardware verification, such as CMake configure and build.
