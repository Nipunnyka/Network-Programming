""" simple aloha simulation through simpy """

#improting libranries
import simpy
import random

#defining constants
UPPER_LIMIT=1000
TIMEOUT=100
MIN_PROCESS=10
MAX_PROCESS=20
MEAN=10
FRAME_TIME=5 #or transmission-time(time taken to generate a frame)
TOT_STATION=3
SIM_TIME=1000
SEED_VALUE=43
e=2.71828

#dfiining station
def station(env,stno,mpipe,ackpipe,timeout,upper_limit):
    while upper_limit>=0:
        while timeout>=0:
            flag=False
            t=random.expovariate(1.0/MEAN)
            yield env.timeout(t)
            #timer at startion starts from here
            print("message sent from station %d at : %f"%(stno,env.now))
            message=(stno,"%d send the message at %f"%(stno,env.now))
            yield env.timeout(FRAME_TIME)
            timeout-=FRAME_TIME
            mpipe.put(message)
            while True:
                if timeout<=0:
                    break
                if ackpipe.items and (ackpipe.items[0]==stno):
                    ack=yield ackpipe.get()
                    flag=True
                    break
                else:
                    yield env.timeout(1)
                    timeout-=1


            if flag==True:
                print("ack received at station %d at : %f"%(stno,env.now))
                break

            else:
                print("timeout of station %d ,initiating retransmission at : %f"%(stno,env.now))
                upper_limit-=timeout

        if flag==True:
            break

#defining receiver
def reciver(env,mpipe,ackpipe,mx):
    #destroying packets if more than 1 at a time
    k=len(mpipe.items)
    if k>1:
        mpipe.items=[]
        print("%d packet_destroyed at %f"%(k,env.now))

    message=yield mpipe.get()
    print("message from station %d received at receiver : %f "%(message[0],env.now))
    mx[0]=max(env.now,mx[0])
    process_time=random.uniform(MIN_PROCESS,MAX_PROCESS)
    yield env.timeout(process_time)
    #print("station %d message's process time %f"%(message[0],process_time))
    print("acknowledgement %d sent from reciver at : %f"%(message[0],env.now))
    ackpipe.put(message[0])



#defining main()
def main():
    random.seed(SEED_VALUE)
    env=simpy.Environment()
    mpipe=simpy.Store(env)
    ackpipe=simpy.Store(env)
    mx=[0.0]
    for i in range(TOT_STATION):
        env.process(station(env,i+1,mpipe,ackpipe,TIMEOUT,UPPER_LIMIT))
    for i in range(TOT_STATION):
        env.process(reciver(env,mpipe,ackpipe,mx))

    env.run(until=SIM_TIME)

    #calculating NORMALIZED CHANNEL TRAFFIC
    ld=TOT_STATION/mx[0]
    G=ld*FRAME_TIME

    #calculating THROUGHPUT
    S=G*(e**(-2*G))
    print("\n\n")
    print("NORMALIZED CHANNEL TRAFFIC : %f"%(G))
    print("THROUGHPUT : %f"%(S))
if(__name__=="__main__"):
    main()
