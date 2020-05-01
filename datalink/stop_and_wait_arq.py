""" stop and wait arq simulation through simpy """

#improting libranries
import simpy
import random

#defining constants
ERROR_CHANNEL=1 #1 for erroneous channel and 0 for non-error channel
SENDER_WINDOW=1
RECEIVER_WINDOW=1
UPPER_LIMIT=300
TIMEOUT=100
MIN_PROCESS=10
MAX_PROCESS=20
BANDWIDTH=int(input("Enter bandwidth of the channel (in bps):"))
FRAME_LENGTH=int(input("Enter length of the frame (in bits):"))
CHANNEL_LENGTH=int(input("Enter length of the channel (in meters): "))
TRANSMISSION_SPEED=int(input("Enter transmission speed (in m/s): "))
print("\n")
TRANSMISSION_TIME=FRAME_LENGTH/BANDWIDTH
PROPOGATION_TIME=CHANNEL_LENGTH/TRANSMISSION_SPEED
a=PROPOGATION_TIME/TRANSMISSION_TIME
EFFICIENCY=1/(1+2*a)
THROUGHPUT=BANDWIDTH*EFFICIENCY
SIM_TIME=1000
SEED_VALUE=43
e=2.71828

#defining messageGenerator function
def messageGenerator(env,i,mpipe,ackpipe,timeout,upper_limit):
    print("message-sequence_no%d sent from station at : %f"%(i,env.now))
    message=(i,"message-sequence_no%d"%(i))
    yield env.timeout(TRANSMISSION_TIME)
    timeout-=TRANSMISSION_TIME
    yield env.timeout(PROPOGATION_TIME)
    timeout-=PROPOGATION_TIME
    mpipe.put(message)


#defining errorInclusion()
def errorInclusion(env,mpipe):
        t=random.randint(0,100)
        if t<30:
            yield mpipe.get()
            yield env.timeout(1)
            print("message/ack dropped due to error")


#dfiining station
def station(env,mpipe,ackpipe,timeout,upper_limit,swindow):
    sequence_no=swindow+1;
    while True:
        print("\n\n---------------------------INITIATING TRANSMISSION--------------------------------------------")

        for i in range(0,sequence_no,swindow):
            upper_limit=UPPER_LIMIT
            timeout=TIMEOUT

            while upper_limit>0:
                flag=False
                #timer at startion starts from here
                for j in range(i,min(i+swindow,sequence_no)):
                    env.process(messageGenerator(env,j,mpipe,ackpipe,timeout,upper_limit))

                while True:
                    if timeout<=0:
                        break
                    if ackpipe.items and (ackpipe.items[0]==(i+1)%sequence_no):
                        if ERROR_CHANNEL:
                            env.process(errorInclusion(env,ackpipe))

                        if ackpipe.items and (ackpipe.items[0]==(i+1)%sequence_no):
                            ack=yield ackpipe.get()
                        else:
                            continue
                        flag=True
                        break
                    else:
                        yield env.timeout(1)
                        timeout-=1


                if flag==True:
                    print("ack-%d received at station at : %f"%((i+1)%sequence_no,env.now))
                    break

                else:
                    print("timeout of message-sequence_no%d ,initiating retransmission at : %f"%(i,env.now))
                    timeout=TIMEOUT
                    upper_limit-=timeout


            if flag==False:
                print("upper_limit of message-sequence_no%d reached at %f ,terminating transmission"%(i,env.now))





#defining receiver
def reciver(env,mpipe,ackpipe,swindow):
    sequence_no=swindow+1
    while True:
        #hamdling error mech
        if ERROR_CHANNEL:
            env.process(errorInclusion(env,mpipe))

        message=yield mpipe.get()
        print("message-sequence_no%d received at receiver at : %f "%(message[0],env.now))
        process_time=random.uniform(MIN_PROCESS,MAX_PROCESS)
        yield env.timeout(process_time)
        print("ack-%d sent from reciver at : %f"%((message[0]+1)%sequence_no,env.now))
        yield env.timeout(PROPOGATION_TIME)
        ackpipe.put((message[0]+1)%sequence_no)



#defining main()
def main():
    random.seed(SEED_VALUE)
    env=simpy.Environment()
    mpipe=simpy.Store(env)
    ackpipe=simpy.Store(env)

    env.process(station(env,mpipe,ackpipe,TIMEOUT,UPPER_LIMIT,SENDER_WINDOW))
    env.process(reciver(env,mpipe,ackpipe,SENDER_WINDOW))

    env.run(until=SIM_TIME)

    print("\n\n---------------------------RESULTS OF TRANSMISSION--------------------------------------------\n")
    print("Transmission Time is : %f"%(TRANSMISSION_TIME))
    print("Propogation Time is : %f"%(PROPOGATION_TIME))
    print("Efficiency is : %f"%(EFFICIENCY))
    print("Effective Bandwidth or Throughput is : %f"%(THROUGHPUT))


if(__name__=="__main__"):
    main()
