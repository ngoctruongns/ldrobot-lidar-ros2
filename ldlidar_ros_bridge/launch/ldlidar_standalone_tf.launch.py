from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    parent_frame_arg = DeclareLaunchArgument(
        'parent_frame',
        default_value='base_link',
        description='Parent frame for LiDAR static transform'
    )

    child_frame_arg = DeclareLaunchArgument(
        'child_frame',
        default_value='lidar_link',
        description='Child frame for LiDAR static transform'
    )

    x_arg = DeclareLaunchArgument('x', default_value='0.0', description='X translation (m)')
    y_arg = DeclareLaunchArgument('y', default_value='0.0', description='Y translation (m)')
    z_arg = DeclareLaunchArgument('z', default_value='0.0', description='Z translation (m)')

    yaw_arg = DeclareLaunchArgument('yaw', default_value='0.0', description='Yaw rotation (rad)')
    pitch_arg = DeclareLaunchArgument('pitch', default_value='0.0', description='Pitch rotation (rad)')
    roll_arg = DeclareLaunchArgument('roll', default_value='0.0', description='Roll rotation (rad)')

    static_tf_node = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='ldlidar_static_tf_publisher',
        arguments=[
            LaunchConfiguration('x'),
            LaunchConfiguration('y'),
            LaunchConfiguration('z'),
            LaunchConfiguration('yaw'),
            LaunchConfiguration('pitch'),
            LaunchConfiguration('roll'),
            LaunchConfiguration('parent_frame'),
            LaunchConfiguration('child_frame'),
        ],
        output='screen'
    )

    return LaunchDescription([
        parent_frame_arg,
        child_frame_arg,
        x_arg,
        y_arg,
        z_arg,
        yaw_arg,
        pitch_arg,
        roll_arg,
        static_tf_node,
    ])
