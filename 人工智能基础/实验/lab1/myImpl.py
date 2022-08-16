import util

"""
Data sturctures we will use are stack, queue and priority queue.

Stack: first in last out
Queue: first in first out
    collection.push(element): insert element
    element = collection.pop() get and remove element from collection

Priority queue:
    pq.update('eat', 2)
    pq.update('study', 1)
    pq.update('sleep', 3)
pq.pop() will return 'study' because it has highest priority 1.

"""

"""
problem is a object has 3 methods related to search state:

problem.getStartState()
Returns the start state for the search problem.

problem.isGoalState(state)
Returns True if and only if the state is a valid goal state.

problem.getChildren(state)
For a given state, this should return a list of tuples, (next_state,
step_cost), where 'next_state' is a child to the current state, 
and 'step_cost' is the incremental cost of expanding to that child.

"""
def myDepthFirstSearch(problem):
    visited = {}
    frontier = util.Stack()

    frontier.push((problem.getStartState(), None))

    while not frontier.isEmpty():
        state, prev_state = frontier.pop()

        if problem.isGoalState(state):#已到达目标结点
            solution = [state]
            while prev_state != None:
                solution.append(prev_state)
                prev_state = visited[prev_state]
            return solution[::-1]                
        
        if state not in visited:
            visited[state] = prev_state

            for next_state, step_cost in problem.getChildren(state):#遍历state状态可以到达的所有状态
                frontier.push((next_state, state))

    return []

def myBreadthFirstSearch(problem):
    # YOUR CODE HERE
    visited={}
    Queue=util.Queue() #BFS需要用队列
    Queue.push((problem.getStartState(),None))

    while not Queue.isEmpty():
        state , prev_state = Queue.pop()
        if problem.isGoalState(state):#已到达目标结点
            solution = [state]
            while prev_state != None:
                solution.append(prev_state)
                prev_state = visited[prev_state]
            return solution[::-1]      
        if state not in visited:#如果该结点没访问，则访问该结点，并把其邻居全部入队
            visited[state]=prev_state
            for next_state,step_cost in problem.getChildren(state):
                Queue.push((next_state,state))
    return []


def myAStarSearch(problem, heuristic):
    # YOUR CODE HERE
    visited={}#维护每个状态的前驱结点
    pq=util.PriorityQueue()
    start_state=problem.getStartState()
    cur_cost=0#记录从初始状态出发,到当前结点的总代价
    pq.update((start_state,None,0),0+heuristic(start_state))
    while not pq.isEmpty():
        state,prev_state,cur_cost=pq.pop()
        if problem.isGoalState(state):#是目标结点，则返回其路径
            solution = [state]
            while prev_state is not None:
                solution.append(prev_state)
                prev_state = visited[prev_state]
            return solution[::-1]
        if state not in visited:
            visited[state]=prev_state
            for next_state,step_cost in problem.getChildren(state):
                next_cost=cur_cost+step_cost
                pq.update((next_state,state,next_cost),next_cost+heuristic(next_state))

    return []

"""
Game state has 4 methods we can use.

state.isTerminated()
Return True if the state is terminated. We should not continue to search if the state is terminated.

state.isMe()
Return True if it's time for the desired agent to take action. We should check this function to determine whether an agent should maximum or minimum the score.

state.getChildren()
Returns a list of legal state after an agent takes an action.

state.evaluateScore()
Return the score of the state. We should maximum the score for the desired agent.

"""
class MyMinimaxAgent():

    def __init__(self, depth):
        self.depth = depth

    def minimax(self, state, depth):#depth是算法搜索的深度
        if state.isTerminated():
            return None, state.evaluateScore()   
        if state.isMe() and depth == 0:#即算法已经不允许继续往下搜索了
            return state, state.evaluateScore()
        best_state, best_score = None, -float('inf') if state.isMe() else float('inf')
        if state.isMe():
            depth = depth - 1
        for child in state.getChildren():
            # YOUR CODE HERE
            _,cur_score = self.minimax(child,depth)
            #对于pacman，要求max
            if state.isMe():
                if best_score<cur_score:
                    best_score=cur_score
                    best_state=child
            else:#对ghost，要min
                if best_score>cur_score:
                    best_score=cur_score
                    best_state=child
        return best_state, best_score

    def getNextState(self, state):
        best_state, _ = self.minimax(state, self.depth)
        return best_state

class MyAlphaBetaAgent():

    def __init__(self, depth):
        self.depth = depth

    def alphabeta_cut(self,state,depth,alpha,beta):
        if state.isTerminated():
            return None, state.evaluateScore()
        if state.isMe() and depth == 0:
            return state, state.evaluateScore()
        best_state, best_score = None, -float('inf') if state.isMe() else float('inf')

        if state.isMe():
            depth=depth-1
        for child in state.getChildren():
            _,cur_score = self.alphabeta_cut(child,depth,alpha,beta)
            if state.isMe():
                if cur_score>beta:
                    return child,cur_score
                alpha=max(cur_score,alpha)
                if best_score<cur_score:
                    best_score=cur_score
                    best_state=child
            else:
                if cur_score<alpha:
                    return child,cur_score
                beta=min(cur_score,beta)
                if best_score>cur_score:
                    best_score=cur_score
                    best_state=child
        return best_state,best_score
    def getNextState(self, state):
        # YOUR CODE HERE
        alpha=-float('inf')#当前为止发现的MAX的最佳值选择
        beta=float('inf')#当前为止发现的MIN结点的最佳选择
        best_state,best_score=self.alphabeta_cut(state, self.depth, alpha, beta)
        return best_state