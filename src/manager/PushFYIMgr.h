/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS Manager.

This class is responsible for Managing CLient Keys
Validation and maintaining statistics for client
reporting.
*********************************************************
*/
#ifndef __PUSHFYI_MANAGER__
#define __PUSHFYI_MANAGER__

#include <algorithm>
#include "Runnable.h"
#include "Thread.h"
#include "Mutex.h"
#include "Event.h"
#include "PriorityQueue.h"
#include "EpollProxy.h"
#include "PubSubRouter.h"
#include "Pushfyi-Defs.h"

/*!
 * PUSH FYI RS's Manager Class
 *
 */
class PushFYIManager : public Runnable, EpollProxy,
								Pushable<Event> {
public:
    typedef std::map<std::string, int> CommandMap;

private:
    /*!
     * PushFYI Manager Constructor
     */
    PushFYIManager();

    /*!
     * Prevent copy of Manager Instance
     */
    PushFYIManager(const PushFYIManager&);

    /*!
     * Self Instance
     */
    static PushFYIManager* mInstance;

    /*!
     * Process the command sent to the PushFYI Manager.
     *
     * This method is called when notification pipe is triggered. It then fetches the
     * commands present in the command queue and process them.
     */
    void processCmd();

    /*!
     * Initialize the list of commands and corresponding codes.
     */
    void initializeCommandMap();

    /*!
     * This method triggers the notification pipe by writing to it's write end.
     */
    void notify();

    /*!
     * Pushes the command depending upon it's priority.
     * @see push
     * @see pushFront
     */
    void submitCmd(Event event, bool isHighPriority = false);

    /*!
     * subscribe for PushFYI Manager topic and subtopic to the PubSubRouter
     */
    void subscribe();

    /*!
     * unsubscribe for PushFYI manager topic and subtopic to the PubSubRouter
     */
    void unsubscribe();

    /*
     * Authorize mgmt client
     */
    void authorizeMgmtClient(Event &);
    void authorizeMgmtClientComplete();
public:

    /*!
     * Interface method to get the class Instance
     */
    static PushFYIManager* getInstance();

    /*!
     * Destructor
     *
     * This must always be virtual.
     */
    virtual ~PushFYIManager();

    /*! Thread Function
     *
     *	Implement the PushFYIManager's main thread.
     */
    void run();

    /*! Start the Manager Thread
     *
     *	Method to start the Manager's main thread to perform IO
     */
    bool online();

    /*! Stop the reader
     *
     *	Method to stop the Manager's main thread. This will stop all IO
     */
    bool offline();

    /*!
     * Process something in idle time.
     *
     * This should not be a very lengthy operation.
     *
     * TODO: This could be a place to submit IO stats to the database.
     * However, in high load scenarios this method may just starve.
     */
    void idle(int);

    /*!
     * Concrete implementation for Pushable::push
     *
     * @param event Request event to be pushed
     */
    virtual void push(Event event);

    /*!
     * Concrete implementation for Pushable::pushFront to push high priority messages.
     * @see Pushable Pushable interface
     *
     * @param event Request event to be pushed.
     *
     * Do not use this method for pushing events on the TCPReader queue without knowing
     * what might happen. Unknowing use of this method can lead to starvation of incoming
     * messages and some other control messages.
     *
     * Avoid it's use unless you know exactly what you are doing.
     */
    virtual void pushFront(Event event);

private:
    /*! Thread object
     *
     * PushFYIManager's thread object
     * @see Thread
     */
    Thread*     mThread;

    /*! Synchronization Mutex
     *
     * pthread_mutex_t wrapper
     */
    Mutex       mMutex;

    bool        mFinished;

    /*! PushFYIManager's Command Queue
     *
     * PushFYI Manager can be controlled dynamically by sending command's to this queue.
     *
     * Command event must contain the following parameters:
     *
     * topic = "pushfyi-manager"
     * subtopic = *
     * command = <command-to-process>
     */
    PriorityQueue<Event>*	mCmdQueue;

    /*!
     * PushFYI Manager Topic String
     */
    string                  mTopic;

    /*!
     * Event Router Instance
     *
     * @see PubSubRouter The main Publish Subscribe Event router.
     *
     * Say the words and remember them : "I would not mess with this class"
     */
    PubSubRouter&   mRouter;

    /*!
     * PushFYI Manager is notified asynchronously of any commands that have been sent to it.
     *
     * This notification is executed to trigger the processing of commands.
     */
    int 		mPipeFd[2];

    /*!
     * PushFYI Manager Command List
     *
     * A map of PushFYI  Manager command code and corresponding string value
     */
    CommandMap	mCommandMap;

    /*!
     * Command codes
     */
    enum CommandType {
    	eDefaultCommand = 0,
    	eAuthorize,
    	eAuthorizeComplete
    };
};

#endif //__PUSHFYI_MANAGER__
