# ESTUN ERI SDK AI Development Guide

This document gives AI coding tools and developers a shared, repository-local
guide to the public ERI API. It does not replace the public headers or site
safety review.

## 1. Facts and Boundaries

Use the following sources together:

- `include/ERIParamManager.h`: public API signatures and documented contracts
- `examples/01_APOS_demo/main.cpp`: joint real-time control flow
- `examples/02_CPOS_demo/main.cpp`: Cartesian real-time control flow
- `examples/03_GETDO_demo/main.cpp`: digital output reading and guarded writing
- `examples/04_STATUS_CALLBACK_demo/main.cpp`: asynchronous robot status
- `README.md`: dependencies, build, and launch instructions

The repository distributes binary SDK libraries, not their implementation
source. Do not invent behavior that cannot be established from these files.
In particular, the current header does not document every unit, valid range,
timeout, lifetime rule, or error code. Ask for authoritative SDK information
when a requested change depends on a missing contract.

## 2. Choose the Closest Example

| Task | Start from | Main APIs |
| --- | --- | --- |
| Joint real-time control | `01_APOS_demo` | `getControlRobot`, `servoToAPOS` |
| Cartesian real-time control | `02_CPOS_demo` | `getWorldCpos`, `servoToCPOS` |
| Read digital output | `03_GETDO_demo` | `getCurrentDO` |
| Write digital output | `03_GETDO_demo` | `setDo` |
| Receive robot status | `04_STATUS_CALLBACK_demo` | `setRobotStatusCallback` |

There is no repository example for `movL`, `servoToOFFSET`, tool/user
modification, or correction parameters. For these APIs, follow the header only
and do not fill contract gaps by analogy.

## 3. Connection Semantics

`setCommunicationParam(robotIp, cmdPort, servoPort, statusPort)` writes the PC
communication parameters. A successful return does not mean that the robot is
connected.

The examples currently use these values only as site-specific placeholders:

```text
robot IP:   172.31.16.227
command:    61210
servo:      61211
status UDP: 61212
```

Do not reuse them without operator confirmation. The README identifies a
successful connection by all three SDK log messages being present:

```text
[INFO] [UDPSocket] connect success
[INFO] [ServoSocket] connect success
[INFO] [cmdSocket] connect success
```

Code can also call `getRobotConnStatus(cmdStatus, servoStatus, udpStatus)`, but
the header does not define the numeric values of the three status outputs.
Do not invent those values.

On Linux, use the generated launch script rather than executing the `_bin`
binary directly. The script sets `LD_LIBRARY_PATH` and changes to the executable
directory so the SDK can find its configuration file.

## 4. Real-Time Motion Workflow

The APOS and CPOS examples establish this workflow:

1. Configure communication with `setCommunicationParam`.
2. Register the status callback when completion status is needed.
3. Confirm the required SDK channels are connected.
4. Acquire control with `getControlRobot`.
5. Configure the sampling period and filter with `setERIMotionParam`.
6. Move to a human-approved start pose with `movJ`.
7. Read the current TCP pose when the Cartesian trajectory needs it.
8. Select the matching servo mode with `startServo`.
9. Send continuous targets with a timestamp advanced by the sample period.
10. Send the final target with `IsEndPoint = true`.
11. Wait for a status received after that final frame to report that motion has
    stopped.
12. Release control with `releaseControlOverRobot`.

The examples use a 4 ms sample period and filter type `0`; these are example
settings, not universal production requirements.

The examples describe `servoToAPOS` and `servoToCPOS` as blocking sends whose
normal call duration is close to the sample period when network communication
is healthy. They do not add another fixed sleep inside the send loop.

### Return values used by the examples

- `getControlRobot`, `setERIMotionParam`, `startServo`, and
  `releaseControlOverRobot`: Boolean success/failure.
- `movJ`: `1` means completed, `0` means executing, and `-1` means command
  failure, as documented by the header.
- `servoToAPOS` and `servoToCPOS`: the public header only documents `-1` as
  failure. Do not assign meaning to other values without more documentation.

Do not release control immediately after sending the endpoint frame. The motion
examples record the current status sequence, then accept only a later callback
that reports `isEndPoint == false`. This avoids accepting an old stopped state.
The field name is potentially misleading: the header documents `true` as
moving and `false` as stopped.

## 5. Units and Data Layout

Only rely on units explicitly documented in `ERIParamManager.h`:

| Data/API | Layout | Units |
| --- | --- | --- |
| `servoToAPOS` target | `[J1 ... J6]` | degrees |
| `servoToCPOS` target | `[x,y,z,a,b,c]` | mm x3, degrees x3; ZYX Euler |
| `servoToOFFSET` target | `[x,y,z,a,b,c]` | mm x3, degrees x3; ZYX fixed |
| `worldCpos` first 6 | `[x,y,z,r,p,y]` | mm x3; rad x3 |
| `RobotStatus::jointValue` | joint values | radians |
| `setERIMotionParam::period` | sampling period | milliseconds |
| `setCorrectParam::alarmThreshold` | correction alarm threshold | millimeters |

The public arrays have capacity 16 where declared, while current six-axis
examples use the first six elements. Preserve the declared buffer size for SDK
output buffers.

The current header does not explicitly define the units accepted by `movJ`,
`movL`, `getWorldCpos`, tool/user data, or correction arrays beyond the entry
noted above. Do not infer these contracts from similarly named fields.

## 6. Status Callback and Concurrency

`setRobotStatusCallback` registers `P_RobotStatusCallback`, which receives a
`RobotStatus` by value. The header states that the callback runs on an
asynchronous thread and must not block.

Inside the callback:

- copy only the needed state;
- use atomics for independent scalar flags/counters, or a mutex for a coherent
  status snapshot;
- avoid network calls, motion commands, console interaction, long computation,
  and waiting;
- perform formatting and user-facing output in another thread.

The status example protects the complete latest status with a mutex. The motion
examples use atomics for the completion flag and status sequence.

`RobotStatus` fields documented by the public header:

- `isRobotError`: whether the robot reports an error;
- `isStartCorrect`: whether correction has started;
- `isEndPoint`: `true` while moving and `false` after stopping;
- `worldCpos`: world-frame Cartesian status;
- `jointValue`: joint status.

## 7. Digital Output APIs

The header exposes:

```cpp
bool setDo(UINT32 port, UINT32 val);
UINT64 getCurrentDO();
UINT32 getCurrentDO(const UINT32 port);
```

`03_GETDO_demo` demonstrates reading `getCurrentDO(4)` and guarded writes with
`setDo`. The example accepts values `0` and `1`, and requires the program to
acquire robot control before writing. The public header does not define whether
control acquisition is a general precondition for `setDo`.

The header does not define SDK-wide valid port ranges, valid output values, bit
ordering for the aggregate value, or how read failures are represented. The
current system configuration has a default writable range of DO12-DO32
inclusive. Changing that range also requires a matching change to the robot's
lower-level configuration file. Treat this as a system configuration
constraint, not an SDK-wide API guarantee. Confirm the deployed configuration
and that the target output cannot trigger hazardous equipment movement before
writing physical outputs.

## 8. Error Handling Expectations

- Check every documented Boolean or failure return before continuing.
- Do not start servo control after control acquisition or parameter setup fails.
- Stop sending targets after `servoTo*` returns `-1`.
- Do not claim that control was released when the release call failed.
- Use `getCurErrMsg` when error details are needed, while respecting its Boolean
  return. The header does not specify the required `errMsg` buffer size.
- Do not silently substitute `forceReleaseControlOverRobot` for normal release;
  its exceptional-use contract is not documented in this repository.

## 9. Safety Review Before Real Motion

Generated motion code is not ready for physical execution until a qualified
person confirms all of the following:

- robot model, axis count, installation, payload, and tool;
- user/tool coordinate frames and orientation convention;
- every position and orientation unit;
- current pose, start pose, trajectory, speed, acceleration, and sample period;
- joint limits, workspace, fixtures, people, and collision risk;
- controller mode, control ownership, network settings, and emergency-stop
  readiness.

Never run a physical motion example as part of automated verification. Build
and static checks are non-hardware verification; actual robot validation must
be an explicit, supervised step.

## 10. Change Verification

For documentation-only changes:

1. Validate Skill frontmatter if a Skill changed.
2. Check Markdown links and formatting with an available Markdown linter.
3. Confirm referenced paths and API names exist.

For C++ or CMake changes:

1. Run the repository's configured lint/static checks, if present.
2. Configure with `cmake -S . -B build`.
3. Build with `cmake --build build -j`.
4. Do not launch motion examples automatically.

If a required binary package is missing, CMake configuration will fail with a
message directing the developer to download the matching release artifact.
