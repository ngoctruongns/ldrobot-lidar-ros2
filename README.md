# LDROBOT LiDAR - Unified Pi/Laptop Workspace

This repository is refactored to support **one branch** and build both sides:

- `ldlidar_dds_server`: DDS publisher (LiDAR -> FastDDS)
- `ldlidar_ros_bridge`: ROS2 bridge (FastDDS -> ROS2 `LaserScan`)
- `ldlidar_component`: shared core logic/libraries used by both

This removes duplicated maintenance across separate Pi/Laptop branches.

## Package layout

- `ldlidar_component` (shared)
  - LiDAR driver library
  - FastDDS message/pub-sub library (`LidarMsg_lib`)
  - Shared tools/logger
  - Shared LiDAR component logic
- `ldlidar_dds_server`
  - Executable: `ldlidar_dds_server_node`
- `ldlidar_ros_bridge`
  - Executable: `ldlidar_ros_bridge_node`
  - Executable: `ldlidar_ros_bridge_debug_subscriber`

## Why keep one branch

Recommended: **yes, keep one branch** for feature development.

- Shared fixes are applied once in `ldlidar_component`.
- DDS server and ROS bridge stay compatible by design.
- CI/build matrix is simpler (`common + dds_server + ros_bridge`).
- Release tagging is clearer than cross-branch cherry-picking.

If needed, keep separate stable release branches (e.g. `release/v1.x`), not role-based branches.

## Prerequisites

### Raspberry Pi (no ROS2 required)

- CMake + C++ compiler
- FastDDS dependencies (`fastrtps`, `fastcdr`)
- `libudev-dev`

### Laptop (ROS2 side)

- ROS2 workspace with `colcon`
- `rosdep`
- FastDDS dependencies (`fastrtps`, `fastcdr`)
- `libudev-dev`

Install base dependency:

```bash
sudo apt update
sudo apt install -y libudev-dev
```

## Build

### Build on Raspberry Pi (standalone, no ROS2)

```bash
cd /ros2_ws/src/ldrobot-lidar-ros2/ldlidar_dds_server
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

Binary output:

- `/ros2_ws/src/ldrobot-lidar-ros2/ldlidar_dds_server/build/ldlidar_dds_server_node`

### Build on Laptop (ROS2)

```bash
cd /ros2_ws
source /opt/ros/$ROS_DISTRO/setup.zsh
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install --packages-select ldlidar_component ldlidar_ros_bridge
source install/local_setup.zsh
```

## Run

### Raspberry Pi side (no ROS2)

```bash
cd /ros2_ws/src/ldrobot-lidar-ros2/ldlidar_dds_server/build
./ldlidar_dds_server_node
```

### Laptop side (ROS2)

Bridge FastDDS -> ROS2:

```bash
cd /ros2_ws
source /opt/ros/$ROS_DISTRO/setup.zsh
source install/local_setup.zsh
ros2 run ldlidar_ros_bridge ldlidar_ros_bridge_node
```

Check ROS2 output:

```bash
ros2 topic list | grep ld_lidar_dds
ros2 topic echo /ld_lidar_dds
```

Optional DDS debug subscriber:

```bash
cd /ros2_ws
source /opt/ros/$ROS_DISTRO/setup.zsh
source install/local_setup.zsh
ros2 run ldlidar_ros_bridge ldlidar_ros_bridge_debug_subscriber
```

### One-command launch (TF + Bridge + RViz2)

Run everything in one command (static TF, DDS->ROS2 bridge, RViz2):

```bash
cd /ros2_ws
source /opt/ros/$ROS_DISTRO/setup.zsh
source install/local_setup.zsh
ros2 launch ldlidar_ros_bridge ldlidar_all_in_one.launch.py
```

RViz config is stored in `ldlidar_ros_bridge/rviz/ldlidar_demo.rviz`.
When the workspace is built with `--symlink-install`, saving from RViz updates the source config directly.

Useful options:

```bash
# Custom TF
ros2 launch ldlidar_ros_bridge ldlidar_all_in_one.launch.py \
  parent_frame:=map \
  child_frame:=ldlidar_frame \
  x:=0.10 y:=0.00 z:=0.20 yaw:=0.0 pitch:=0.0 roll:=0.0

# Disable RViz2
ros2 launch ldlidar_ros_bridge ldlidar_all_in_one.launch.py start_rviz:=false
```

### Independent TF only (optional)

If you only want a static TF publisher (without bridge/RViz2):

```bash
cd /ros2_ws
source /opt/ros/$ROS_DISTRO/setup.zsh
source install/local_setup.zsh
ros2 launch ldlidar_ros_bridge ldlidar_standalone_tf.launch.py
```

Default TF is:

- `base_link -> ldlidar_frame`

Example with custom frame and offset:

```bash
ros2 launch ldlidar_ros_bridge ldlidar_standalone_tf.launch.py \
  parent_frame:=map \
  child_frame:=ldlidar_frame \
  x:=0.10 y:=0.00 z:=0.20 yaw:=0.0 pitch:=0.0 roll:=0.0
```

For RViz2 quick test:

- Set `Fixed Frame` to `base_link` (or your `parent_frame`).
- Add display type `LaserScan` and select topic `/ld_lidar_dds`.

## DDS compatibility contract (Pi <-> Laptop)

Current fixed values expected on both sides:

- DDS Domain ID: `0`
- DDS Topic: `LidarMsgTopic`
- DDS Type: `LidarMessage` (`ldlidar_fastdds/LidarMsg.idl`)

## Suggested next improvements

1. Add ROS2 parameters for bridge output (`frame_id`, `range_min/max`, bins, output topic).
1. Add launch file for `ldlidar_dds_server` publisher startup.
1. Add CI build matrix for `common`, `dds_server`, `ros_bridge` targets.
1. Add QoS and reconnect diagnostics for DDS link status.

## License

Apache License 2.0. See `LICENSE`.
