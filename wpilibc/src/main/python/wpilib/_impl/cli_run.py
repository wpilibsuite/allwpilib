class Main:
    """
    Executes the robot code using the currently installed HAL (this is probably not what you want unless this is controlling the physical robot)
    """

    def __init__(self, parser):
        pass

    def run(self, options, robot_class, **static_options):
        return robot_class.main(robot_class)
