import rospy
from mblocks_wifi.msg import Move

NODE_NAME = 'aggregation_planner'

class Aggregation(object):
    def __init__(self, cubes):
        rospy.init_node(NODE_NAME, anonymous=False)

        self.planner_pub = rospy.Publisher('core/planner', Move, queue_size=1)
        
        r = rospy.Rate(10)
        while not rospy.is_shutdown():
            r.sleep()

    def find_move(self):
        pass
