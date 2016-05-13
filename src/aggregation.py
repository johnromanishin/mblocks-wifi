import rospy
from mblocks_wifi.msg import Move

NODE_NAME = 'aggregation_planner'

class Aggregation(object):
    def __init__(self, cubes):
        rospy.init_node(NODE_NAME, anonymous=False)

        self.stat_sub = rospy.Subscriber('cube1/status', Status, self.status_callback)
        self.planner_pub = rospy.Publisher('core/planner', Move, queue_size=1)
        
        r = rospy.Rate(10)
        while not rospy.is_shutdown():
            r.sleep()

    def status_callback(self, data):
        self.light_readings = data.face_light
        self.magnet_reading = data.magnet_sense
        # self.neighbors = data.neighbors
        
    def find_move(self):
        orientation = self._find_orientation()
        
    def _find_orientation(self):
        pass
