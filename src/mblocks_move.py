#!/usr/bin/env python

import collections

import rospy
from std_msgs.msg import String
from mblocks_wifi.msg import Move, Status

NODE_NAME = 'move_control'

class Mover():
    def __init__(self, cubes):
        rospy.init_node(NODE_NAME, anonymous=False)

        self.cmd_ia = rospy.Publisher('cube1/cmd_ia', String, queue_size=1)
        self.cmd_cp = rospy.Publisher('cube1/cmd_cp', String, queue_size=1)
        
        self.ser_sub = rospy.Subscriber('cube1/serial', String, self.serial_callback)
        self.stat_sub = rospy.Subscriber('cube1/status', Status, self.status_callback)

        self.planner_sub = rospy.Subscriber('core/planner', Move, self.move_callback)

        r = rospy.Rate(10)
        while not rospy.is_shutdown():
            r.sleep()

    def move_callback(self, data):
        cmd_str = moves[data.cube][data.type, data.direction]
        if data.type == 'change_plane':
            self.cmd_cp.publish(cmd_str)
        else:
            self.cmd_ia.publish(cmd_str)

    def status_callback(self, data):
        print 'Cube status:'
        print 'Light readings: {}'.format(data.face1_light)
        print 'Magnet sensor: {}'.format(data.magnet_sense)

    def serial_callback(self, data):
        print 'Serial callback: {}'.format(data.data)
        if 'IA SUCCESS' in data.data:
            pass
        elif 'IA FAILURE' in data.data:
            pass
        elif 'CP SUCCESS' in data.data:
            pass
        elif 'CP FAILURE' in data.data:
            pass
            
