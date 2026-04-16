import os
from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    installed_rviz_config_file = os.path.join(
        get_package_share_directory('ldlidar_ros_bridge'),
        'rviz',
        'ldlidar_demo.rviz'
    )
    source_rviz_config_file = os.path.abspath(
        os.path.join(
            os.path.dirname(os.path.realpath(__file__)),
            '..',
            '..',
            'rviz',
            'ldlidar_demo.rviz'
        )
    )
    rviz_config_file = (
        source_rviz_config_file
        if os.path.exists(source_rviz_config_file)
        else installed_rviz_config_file
    )

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

    start_rviz_arg = DeclareLaunchArgument(
        'start_rviz',
        default_value='true',
        description='Set false to disable RViz2 startup'
    )

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

    ros_bridge_node = Node(
        package='ldlidar_ros_bridge',
        executable='ldlidar_ros_bridge_node',
        name='ldlidar_ros_bridge_node',
        output='screen'
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_file],
        output='screen',
        condition=IfCondition(LaunchConfiguration('start_rviz'))
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
        start_rviz_arg,
        static_tf_node,
        ros_bridge_node,
        rviz_node,
    ])
