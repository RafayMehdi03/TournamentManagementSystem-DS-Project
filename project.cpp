#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
// NO <vector>, NO <unordered_map>, NO <queue> (for heap)
using namespace std;

class Team {
public:
    string name;
    float rating;
    int wins;
    int losses;

    Team() : name(""), rating(0), wins(0), losses(0) {}

    Team(string n, float r) : name(n), rating(r), wins(0), losses(0) {}

    void show() const {
        cout << "Team: " << name
             << " | Rating: " << rating
             << " | W-L: " << wins << "-" << losses << endl;
    }
};

struct Node {
    Team data;
    Node* next;
    Node(Team t) : data(t), next(NULL) {}
};

class LinkedList {
public:
    Node* head;
    LinkedList() : head(NULL) {}

    void addTeam(Team t) {
        Node* n = new Node(t);
        if (!head) head = n;
        else {
            Node* t2 = head;
            while (t2->next) t2 = t2->next;
            t2->next = n;
        }
    }

    void showAll() const {
        if (!head) {
            cout << "No teams in list." << endl;
            return;
        }
        Node* t = head;
        while (t) {
            t->data.show();
            t = t->next;
        }
    }

    int countTeams() const {
        int c = 0;
        Node* t = head;
        while (t) { c++; t = t->next; }
        return c;
    }

    void toArray(Team* arr, int &size) {
        size = 0;
        Node* t = head;
        while (t) {
            arr[size++] = t->data;
            t = t->next;
        }
    }
    
    void clear() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = NULL;
    }
};


// Using an array of linked lists for collision handling. O(1) average lookup.
class TeamHashTable {
private:
    static const int TABLE_SIZE = 101; // Prime number for better distribution
    LinkedList buckets[TABLE_SIZE]; // Array of Linked Lists

    // Simple Hash Function: Sum of ASCII values % TABLE_SIZE
    int hashFunction(const string& key) const {
        int hashVal = 0;
        for (char ch : key) {
            hashVal += ch;
        }
        return hashVal % TABLE_SIZE;
    }

public:
    // Add/Update Team (Insertion)
    void insert(const Team& t) {
        int index = hashFunction(t.name);
        
        // Check if team already exists in the bucket (basic find)
        Node* current = buckets[index].head;
        while (current) {
            if (current->data.name == t.name) {
                current->data = t; // Update existing
                return;
            }
            current = current->next;
        }
        
        // Add new team to the front of the list (insertion)
        buckets[index].addTeam(t);
    }

    // Retrieve Team (Lookup - O(1) average)
    Team* find(const string& name) {
        int index = hashFunction(name);
        Node* current = buckets[index].head;
        while (current) {
            if (current->data.name == name) {
                return &(current->data); // Return pointer to data
            }
            current = current->next;
        }
        return NULL; // Not found
    }

    // Convert all teams to a dynamic array (Needed for Leaderboard/Heap)
    void toArray(Team*& arr, int& size) {
        // First, count all teams
        size = 0;
        for (int i = 0; i < TABLE_SIZE; ++i) {
            size += buckets[i].countTeams();
        }

        // Allocate memory for the array
        if (arr) delete[] arr; // Clean up previous allocation if any
        arr = new Team[size]; 
        
        // Copy teams to the array
        int idx = 0;
        for (int i = 0; i < TABLE_SIZE; ++i) {
            Node* current = buckets[i].head;
            while (current) {
                arr[idx++] = current->data;
                current = current->next;
            }
        }
    }
};

class Queue {
public:
    struct QNode {
        Team t;
        QNode* next;
        QNode(Team a) : t(a), next(NULL) {}
    };

    QNode *front, *rear;

    Queue() : front(NULL), rear(NULL) {}

    bool empty() const { return front == NULL; }

    void enqueue(Team a) {
        QNode* n = new QNode(a);
        if (!rear) { front = rear = n; return; }
        rear->next = n;
        rear = n;
    }

    Team dequeue() {
        if (empty()) throw runtime_error("Queue is empty.");
        QNode* temp = front;
        Team x = temp->t;
        front = front->next;
        if (!front) rear = NULL;
        delete temp;
        return x;
    }

    void clear() {
        while (!empty()) dequeue();
    }
};


class Stack {
public:
    struct SNode {
        string winner;
        string loser;
        float ratingChange;
        SNode* next;
        SNode(string w, string l, float rc) : winner(w), loser(l), ratingChange(rc), next(NULL) {}
    };

    SNode* top;

    Stack() : top(NULL) {}

    bool empty() const { return top == NULL; }

    void push(string w, string l, float rc) {
        SNode* n = new SNode(w, l, rc);
        n->next = top;
        top = n;
    }

    SNode pop() {
        if (empty()) throw runtime_error("Stack is empty.");
        SNode* t = top;
        SNode r = *t;
        top = top->next;
        delete t;
        return r;
    }
};


class BST {
public:
    struct BNode {
        string win, lose;
        BNode* left;
        BNode* right;
        BNode(string w, string l) : win(w), lose(l), left(NULL), right(NULL) {}
    };

    BNode* root;

    BST() : root(NULL) {}

    void insert(string w, string l) {
        BNode* n = new BNode(w, l);
        if (!root) { root = n; return; }

        BNode* t = root;
        while (true) {
            // Sorting by winner name for Inorder traversal
            if (w < t->win) {
                if (!t->left) { t->left = n; break; }
                t = t->left;
            } else {
                if (!t->right) { t->right = n; break; }
                t = t->right;
            }
        }
    }

    void inorder(BNode* r) {
        if (!r) return;
        inorder(r->left);
        cout << r->win << " defeated " << r->lose << endl;
        inorder(r->right);
    }

    void show() {
        if (!root) cout << "No results." << endl;
        else inorder(root);
    }
};


class MaxHeap {
private:
    Team* heapArr;
    int capacity;
    int size;

    void heapifyUp(int index) {
        while (index > 0 && heapArr[index].rating > heapArr[(index - 1) / 2].rating) {
            // Swap if child is greater than parent
            swap(heapArr[index], heapArr[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void heapifyDown(int index) {
        int maxIndex = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heapArr[left].rating > heapArr[maxIndex].rating)
            maxIndex = left;
        if (right < size && heapArr[right].rating > heapArr[maxIndex].rating)
            maxIndex = right;

        if (index != maxIndex) {
            swap(heapArr[index], heapArr[maxIndex]);
            heapifyDown(maxIndex);
        }
    }

public:
    MaxHeap(int cap) : capacity(cap), size(0) {
        heapArr = new Team[capacity];
    }
    ~MaxHeap() { delete[] heapArr; }

    bool empty() const { return size == 0; }

    void insert(const Team& t) {
        if (size == capacity) {
            cerr << "Heap capacity reached." << endl;
            return;
        }
        heapArr[size] = t;
        heapifyUp(size);
        size++;
    }

    Team extractMax() {
        if (empty()) throw runtime_error("Heap is empty.");
        Team maxTeam = heapArr[0];
        heapArr[0] = heapArr[--size];
        heapifyDown(0);
        return maxTeam;
    }
};


class Graph {
private:
    struct DefeatNode {
        string loserName;
        DefeatNode* next;
        DefeatNode(const string& name) : loserName(name), next(NULL) {}
    };

    DefeatNode** adjList; // Dynamic array of DefeatNode pointers
    string* teamNames;    // Array to map index to team name
    int maxTeams;         
    int teamCount;        

    int getIndex(const string& name) const {
        for (int i = 0; i < teamCount; ++i) {
            if (teamNames[i] == name) return i;
        }
        return -1;
    }

    void resizeArrays(int newSize) {
        if (newSize <= maxTeams) return;

        DefeatNode** newAdjList = new DefeatNode*[newSize];
        string* newTeamNames = new string[newSize];

        for (int i = 0; i < maxTeams; ++i) {
            newAdjList[i] = adjList[i];
            newTeamNames[i] = teamNames[i];
        }
        for (int i = maxTeams; i < newSize; ++i) {
            newAdjList[i] = NULL;
            newTeamNames[i] = "";
        }

        delete[] adjList;
        delete[] teamNames;
        adjList = newAdjList;
        teamNames = newTeamNames;
        maxTeams = newSize;
    }

    bool DFS_check(int currentIdx, int targetIdx, bool* visited) {
        if (currentIdx == targetIdx) return true;
        visited[currentIdx] = true;

        DefeatNode* current = adjList[currentIdx];
        while (current) {
            int neighborIdx = getIndex(current->loserName);
            if (neighborIdx != -1 && !visited[neighborIdx]) {
                if (DFS_check(neighborIdx, targetIdx, visited)) return true;
            }
            current = current->next;
        }
        return false;
    }

public:
    Graph(int initialSize = 10) : maxTeams(initialSize), teamCount(0) {
        adjList = new DefeatNode*[maxTeams];
        teamNames = new string[maxTeams];
        for (int i = 0; i < maxTeams; ++i) {
            adjList[i] = NULL;
            teamNames[i] = "";
        }
    }
    ~Graph() {
        for (int i = 0; i < teamCount; ++i) {
            DefeatNode* current = adjList[i];
            while (current) {
                DefeatNode* next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] adjList;
        delete[] teamNames;
    }

    void addTeam(const string& name) {
        if (getIndex(name) != -1) return; // Team already tracked
        if (teamCount == maxTeams) {
            resizeArrays(maxTeams * 2);
        }
        teamNames[teamCount] = name;
        adjList[teamCount] = NULL;
        teamCount++;
    }

    void addMatch(const string& winner, const string& loser) {
        if (loser == "bye") return;

        addTeam(winner);
        addTeam(loser);

        int winnerIdx = getIndex(winner);
        if (winnerIdx == -1) return; 

        // Add edge: winner -> loser
        DefeatNode* newNode = new DefeatNode(loser);
        newNode->next = adjList[winnerIdx];
        adjList[winnerIdx] = newNode;
    }

    // ADDED: Missing showGraph function
    void showGraph() const {
        cout << endl << "=== PERFORMANCE GRAPH (Adjacency List) ===" << endl;
        bool empty = true;
        for (int i = 0; i < teamCount; ++i) {
            if (adjList[i]) {
                empty = false;
                cout << teamNames[i] << " defeated -> ";
                DefeatNode* curr = adjList[i];
                while (curr) {
                    cout << curr->loserName << (curr->next ? ", " : "");
                    curr = curr->next;
                }
                cout << endl;
            }
        }
        if (empty) cout << "No matches recorded in graph yet." << endl;
        cout << "==========================================" << endl;
    }

    void checkIndirectWin(const string& teamA, const string& teamB) {
        int idxA = getIndex(teamA);
        int idxB = getIndex(teamB);

        if (idxA == -1 || idxB == -1) {
            cout << "One or both teams not found in the graph." << endl;
            return;
        }

        bool* visited = new bool[teamCount];
        for (int i = 0; i < teamCount; ++i) visited[i] = false;

        if (DFS_check(idxA, idxB, visited)) {
            cout << teamA << " has defeated " << teamB << " (directly or indirectly - via DFS)." << endl;
        } else {
            cout << teamA << " has NOT defeated " << teamB << " (directly or indirectly)." << endl;
        }
        delete[] visited;
    }
};


class TournamentTree {
public:
    struct TNode {
        string teamName;
        TNode* left;
        TNode* right;
        TNode(string n) : teamName(n), left(NULL), right(NULL) {}
    };
    TNode* root;

    TournamentTree() : root(NULL) {}

    void printInorder(TNode* r) {
        if (!r) return;
        printInorder(r->left);
        cout << r->teamName << " ";
        printInorder(r->right);
    }
};


void merge(Team arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    Team* L = new Team[n1];
    Team* R = new Team[n2];

    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i].rating >= R[j].rating) { 
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) { arr[k] = L[i]; i++; k++; }
    while (j < n2) { arr[k] = R[j]; j++; k++; }
    
    delete[] L;
    delete[] R;
}

void mergeSort(Team arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}


class Tournament {
public:
    LinkedList teamsLL; 
    TeamHashTable teamsHT; 
    Queue matches;
    Stack undo; 
    BST results; 
    Graph performanceGraph; 
    TournamentTree tourneyTree; 
    float ratingBonus = 1.5;

    Team* allTeamsArray = NULL; 
    int currentTeamCount = 0;

    Tournament() {
        tourneyTree.root = new TournamentTree::TNode("Champion");
    }
    ~Tournament() {
        if (allTeamsArray) delete[] allTeamsArray;
    }

    /* ----- REGISTRATION ----- */
    void registerTeam(string n, float r) {
        if (teamsHT.find(n)) { 
            cout << "Team already exists." << endl;
            return;
        }
        Team newTeam(n,r);
        teamsHT.insert(newTeam); 
        teamsLL.addTeam(newTeam); 
        performanceGraph.addTeam(n); 
        cout << n << " registered successfully." << endl;
    }

    /* ----- SHOW TEAMS ----- */
    void showTeams() {
        cout << endl << "=== REGISTERED TEAMS ===" << endl;
        teamsLL.showAll();
        cout << "=========================" << endl;
    }

    /* ----- PREDICTION SYSTEM ----- */
    void predictWinner() {
        cout << endl << "=== WINNER PREDICTION SYSTEM ===" << endl;
        if (teamsLL.countTeams() < 2) {
            cout << "Not enough teams for prediction." << endl;
            return;
        }
        
        // Simple prediction based on current top rating
        Team* currentMax = NULL;
        teamsHT.toArray(allTeamsArray, currentTeamCount); 
        for (int i = 0; i < currentTeamCount; ++i) {
            if (!currentMax || allTeamsArray[i].rating > currentMax->rating) {
                currentMax = &allTeamsArray[i];
            }
        }

        if (currentMax) {
            cout << "Prediction based on current max rating: " << currentMax->name << endl;
        }
        cout << "================================" << endl;
    }

    /* ----- SCHEDULE MATCHES ----- */
    void schedule() {
        matches.clear();
        teamsHT.toArray(allTeamsArray, currentTeamCount); 
        
        if (currentTeamCount < 2) {
             cout << "Need at least 2 teams to schedule matches." << endl;
             return;
        }

        MaxHeap maxHeap(currentTeamCount); 
        for (int i = 0; i < currentTeamCount; ++i) {
            maxHeap.insert(allTeamsArray[i]);
        }

        Team* sortedTeams = new Team[currentTeamCount];
        int sortIdx = 0;
        while (!maxHeap.empty()) { 
            sortedTeams[sortIdx++] = maxHeap.extractMax();
        }

        cout << endl << "=== MATCHES SCHEDULED (Seeded by Rating) ===" << endl;

        for (int i = 0; i < currentTeamCount; i += 2) {
            matches.enqueue(sortedTeams[i]);
            if (i + 1 < currentTeamCount) {
                matches.enqueue(sortedTeams[i+1]);
                cout << "Match " << (i/2) + 1 << ": "
                     << sortedTeams[i].name << " (Seed " << i + 1 << ") vs "
                     << sortedTeams[i+1].name << " (Seed " << i + 2 << ")" << endl;
            } else {
                cout << "Match " << (i/2) + 1 << ": "
                     << sortedTeams[i].name << " gets a BYE" << endl;
            }
        }
        delete[] sortedTeams;

        cout << "============================================" << endl;

        char ch;
        cout << "Simulate these matches now? (y/n): ";
        cin >> ch;

        if (ch == 'y' || ch == 'Y')
            simulate();
        else
            cout << "Returning to main menu..." << endl;
    }

    /* ----- SIMULATE MATCHES ----- */
    void simulate() {
        if (matches.empty()) {
            cout << "No matches scheduled." << endl;
            return;
        }

        cout << endl << "=== MATCH SIMULATION ===" << endl;

        while (!matches.empty()) {
            Team t1 = matches.dequeue();
            Team* w, * l; 

            // BYE Handling
            if (matches.empty()) {
                w = teamsHT.find(t1.name); 

                if (w) {
                    w->wins++;
                    w->rating += ratingBonus;
                    cout << w->name << " gets a BYE (wins automatically)." << endl;
                    results.insert(w->name, "bye"); 
                    undo.push(w->name, "bye", ratingBonus); 
                }
                break;
            }

            Team t2 = matches.dequeue();

            // rating-based win probability
            float total = t1.rating + t2.rating;
            if (total <= 0) total = 1;
            float prob = t1.rating / total; 
            string W_name = ((float)rand() / RAND_MAX < prob) ? t1.name : t2.name;
            string L_name = (W_name == t1.name ? t2.name : t1.name);
            
            w = teamsHT.find(W_name); 
            l = teamsHT.find(L_name); 

            if (w && l) {
                w->wins++;
                w->rating += ratingBonus;
                l->losses++;

                cout << W_name << " defeated " << L_name << endl;

                results.insert(W_name, L_name); 
                performanceGraph.addMatch(W_name, L_name); 
                undo.push(W_name, L_name, ratingBonus); 
            }
        }

        cout << "All matches simulated!" << endl;
        cout << "=========================" << endl;
    }

    /* ----- UNDO LAST MATCH ----- */
    void undoLast() {
        if (undo.empty()) {
            cout << "Nothing to undo." << endl;
            return;
        }

        Stack::SNode r = undo.pop();
        
        Team* w = teamsHT.find(r.winner);
        Team* l = (r.loser == "bye") ? NULL : teamsHT.find(r.loser);

        if (w) {
            w->wins--;
            w->rating -= r.ratingChange;
        }

        if (l) l->losses--;

        cout << "Undo complete: " << r.winner << " vs " << r.loser << endl;
    }

    /* ----- LEADERBOARD ----- */
    void leaderboard() {
        teamsHT.toArray(allTeamsArray, currentTeamCount); 

        if (currentTeamCount == 0) {
            cout << "No teams." << endl;
            return;
        }

        mergeSort(allTeamsArray, 0, currentTeamCount - 1);

        cout << endl << "=== LEADERBOARD (Sorted by Rating) ===" << endl;
        for (int i=0; i<currentTeamCount; i++)
            allTeamsArray[i].show();

        cout << "======================================" << endl;
    }

    /* ----- RESULTS ----- */
    void showResults() {
        cout << endl << "=== MATCH RESULTS (BST) ===" << endl;
        results.show();
        cout << "===========================" << endl;
    }
    
    /* ----- PERFORMANCE TRACKING ----- */
    void showPerformance() {
        performanceGraph.showGraph();
        string teamA, teamB;
        cout << "Check for indirect win. Team A: ";
        cin.ignore();
        getline(cin, teamA);
        cout << "Team B: ";
        getline(cin, teamB);
        performanceGraph.checkIndirectWin(teamA, teamB); 
    }

    /* ----- SAVE/LOAD TEAMS ----- */
    void saveTeams() {
        // 1. Refresh the internal array so we are saving the latest data
        teamsHT.toArray(allTeamsArray, currentTeamCount);

        if (currentTeamCount == 0) {
            cout << "[Save] No teams to save." << endl;
            return;
        }

        ofstream f("teams.txt");
        if (!f.is_open()) {
            cout << "[Save] Error: Could not create teams.txt!" << endl;
            return;
        }

        cout << "[Save] Saving " << currentTeamCount << " teams..." << endl;

        // 2. Write data: Name on one line, Stats on the next
        for(int i = 0; i < currentTeamCount; ++i) {
            f << allTeamsArray[i].name << endl;
            f << allTeamsArray[i].rating << " " 
              << allTeamsArray[i].wins << " " 
              << allTeamsArray[i].losses << endl;
        }
        
        f.close();
        cout << "[Save] Success! Data written to teams.txt." << endl;
    }

   void loadTeams() {
        ifstream f("teams.txt");
        if (!f.is_open()) {
            cout << "[Load] No saved file found. Starting with 0 teams." << endl;
            return; 
        }

        string n; 
        float r; 
        int w, l;
        int loadedCount = 0;
        
        // Clear existing data to avoid duplicates
        teamsLL.clear(); 
        
        cout << "[Load] Reading file..." << endl;

        // Loop: Try to read the Name line
        while(getline(f, n)) {
            // Skip empty lines (common at end of files)
            if (n.empty()) continue;

            // Try to read the Stats line
            if (!(f >> r >> w >> l)) {
                break; // Stop if stats are missing/corrupt
            }
            
            // IMPORTANT: Consume the leftover newline character so the next Name read works
            f.ignore(1000, '\n'); 

            // Create and Insert Team
            Team t(n,r);
            t.wins = w; 
            t.losses = l;
            
            teamsLL.addTeam(t);
            teamsHT.insert(t);
            performanceGraph.addTeam(n); // Ensure graph knows about the team
            
            loadedCount++;
        }
        
        f.close();
        cout << "[Load] Success! Loaded " << loadedCount << " teams." << endl;
    }
};


int main() {
    srand(time(0));

    Tournament t;
    
    // Auto-load on startup (optional, you can remove this line if you ONLY want manual load)
    t.loadTeams(); 
    
    int c;

    while (true) {
        cout << endl;
        cout << "===== TOURNAMENT MENU =====" << endl;
        cout << "1.  Register Team" << endl;
        cout << "2.  Show Teams" << endl;
        cout << "3.  Schedule Matches" << endl;
        cout << "4.  Simulate Matches" << endl;
        cout << "5.  Undo Last Match" << endl;
        cout << "6.  Leaderboard" << endl;
        cout << "7.  Show Results" << endl;
        cout << "8.  Show Graph/Check Indirect Win" << endl;
        cout << "9.  Predict Winner" << endl;
        cout << "10. Save Teams to File" << endl; // NEW OPTION
        cout << "11. Load Teams from File" << endl; // NEW OPTION
        cout << "0.  Exit" << endl;
        cout << "Enter Choice: ";

        cin >> c;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Try again." << endl;
            continue;
        }

        if (c == 1) {
            string n; float r;
            cin.ignore(); 
            cout << "Team Name: ";
            getline(cin, n);
            cout << "Team Rating (float): ";
            cin >> r;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid rating. Try again." << endl;
                continue;
            }
            t.registerTeam(n,r);
        }
        else if (c == 2) t.showTeams();
        else if (c == 3) t.schedule();
        else if (c == 4) t.simulate();
        else if (c == 5) t.undoLast();
        else if (c == 6) t.leaderboard();
        else if (c == 7) t.showResults();
        else if (c == 8) t.showPerformance();
        else if (c == 9) t.predictWinner();
        
        // --- NEW OPTIONS ---
        else if (c == 10) {
            t.saveTeams();
        }
        else if (c == 11) {
            cout << "Warning: Loading will overwrite unsaved current data." << endl;
            t.loadTeams();
        }
        // -------------------

        else if (c == 0) {
            // Ask if they want to save before exiting
            char s;
            cout << "Save data before exiting? (y/n): ";
            cin >> s;
            if(s == 'y' || s == 'Y') t.saveTeams();
            
            cout << "Exiting..." << endl;
            break;
        }
        else cout << "Invalid choice." << endl;
    }

    return 0;

}
