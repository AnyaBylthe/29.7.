#include <iostream>
#include <mutex>

struct Node {
  int value;
  Node* next;
  std::mutex* node_mutex;
};

class FineGrainedQueue {
 private:
  Node* head;
  std::mutex* queue_mutex;

 public:
  FineGrainedQueue() : head(nullptr), queue_mutex(new std::mutex) {}
  ~FineGrainedQueue() {
    delete queue_mutex;
    while (head != nullptr) {
      Node* temp = head;
      head = head->next;
      delete temp->node_mutex;
      delete temp;
    }
  }

  void insertIntoMiddle(int value, int pos);

  Node* getNode(int index) const {
    Node* current = head;
    for (int i = 0; i < index && current != nullptr; ++i) {
      current = current->next;
    }
    return current;
  }
};

void FineGrainedQueue::insertIntoMiddle(int value, int pos) {
  Node* newNode = new Node;
  newNode->value = value;
  newNode->next = nullptr;
  newNode->node_mutex = new std::mutex;

  std::lock_guard<std::mutex> queue_lock(*queue_mutex);

  Node* current = head;
  Node* previous = nullptr;
  int currentPos = 0;
  while (current != nullptr && currentPos < pos) {
    previous = current;
    current = current->next;
    currentPos++;
  }

  if (previous != nullptr) {
    std::lock_guard<std::mutex> prev_lock(*previous->node_mutex);
  }
  if (current != nullptr) {
    std::lock_guard<std::mutex> current_lock(*current->node_mutex);
  }

  if (previous == nullptr) {
    newNode->next = head;
    head = newNode;
  } else {
    previous->next = newNode;
    newNode->next = current;
  }
}

int main() {
  FineGrainedQueue queue;

  queue.insertIntoMiddle(1, 0);
  queue.insertIntoMiddle(2, 1);
  queue.insertIntoMiddle(3, 1);

  Node* current = queue.getNode(0);
  while (current != nullptr) {
    std::cout << current->value << " ";
    current = current->next;
  }
  std::cout << std::endl;

  return 0;
}
