
import queue

class StablePriorityQueue:
    """
    A class that implements a priority queue that preserves the ordering in case of same priorities.
    """
    
    def __init__(self, maxsize=0):
        self._counter = 0
        self._queue = queue.PriorityQueue(maxsize)
        
    def put(self, item):
        """
        Insert a new item into the priority queue.
        """
        assert isinstance(item, tuple) and len(item) == 2, "Item must be a tuple of the form: (priority, object)"
        priority, object = item
        self._queue.put((priority, self._counter, object))
        self._counter += 1
        
    def get(self):
        """
        Retrieve and remove the item with the highest priority from the priority queue.
        """
        priority, _, object = self._queue.get()
        return priority, object
    
    def empty(self):
        """
        Check if the priority queue is empty.
        """
        return self._queue.empty()
