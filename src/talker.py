#!/usr/bin/env python

import collections

import rospy
from std_msgs.msg import String, Int32

NODE_NAME = 'move_control'

class Move():
    def __init__(self):
        self.mag_q = deque()
        
        rospy.init_node(NODE_NAME, anonymous=False)

        self.cmd_ia = rospy.Publisher('cmd_ia', String, queue_size=1)
        self.cmd_cp = rospy.Publisher('cmd_cp', String, queue_size=1)

        self.mag_sub = rospy.Subscriber('mag', Int32, self.mag_callback)

        r = rospy.Rate(10)
        while not rospy.is_shutdown():
            self.move()
            r.sleep()

    def move(self):
        fwd_cmd = 'ia f 6000 2500 20'

        if len(self.mag_q) >= 2:
            p, q = self.mag_q.pop(), self.mag_q.pop()
            if abs(p - q) < EPS:
                self.mag_q.clear()
                self.cmd_fwd.publish(fwd_cmd)
            else:
                self.mag_q.extend([p, q])
            
    def mag_callback(self, data):
        if len(self.mag_q) >= 2:
            self.mag_q.popleft()
        self.mag_q.append(data.data)
        
if __name__ == '__main__':
    Move()
