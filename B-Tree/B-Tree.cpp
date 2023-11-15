#include <iostream>
#include <sstream>
#include <string>

using namespace std;

template <class K, class D, int M> // K: key, D: data, M: degree of BTree
class BTree {
    /// Convention: Left sub-tree < Root's key <= Right sub-tree

public:
    class Entry;
    class Node;

private:
    Node* root;

public:
    BTree() : root(0) {};
    ~BTree() {}

    ///////////////////////////////////////////////////
    ///             CLASS `Entry`                   ///
    ///////////////////////////////////////////////////
public:
    class Entry {
    private:
        K key;
        D data;
        Node* rightPtr;

        friend class BTree<K, D, M>;

    public:
        Entry(K key = K{}, D value = D{}) : key(key), data(value), rightPtr(0) {}
        ~Entry() {}

        string toString() {
            stringstream ss;
            ss << "<"
                << this->key << ","
                << this->data
                << ">";
            return ss.str();
        }

    };

    ///////////////////////////////////////////////////
    ///             CLASS `Node`                    ///
    ///////////////////////////////////////////////////
public:
    class Node {
    private:
        Node* firstPtr;
        int numEntries;
        Entry entries[M - 1];

        friend class BTree<K, D, M>;

    public:
        Node() : firstPtr(0), numEntries(0) {};
        ~Node() { }

        bool isFull() {
            return (numEntries >= M - 1);
        }

        /// BEGIN STUDENT CODE
        string toString() {
            stringstream ss;
            ss << "[(" << numEntries << ")";
            for (int i = 0; i < numEntries; i++) {
                ss << entries[i].toString();
            }
            ss << "]";
            return ss.str();
        }
        /// END STUDENT CODE
    };

    /////////////////////////////////////////////////////////////
    ///         CLASS `BTree`: method run sample test         ///
    /////////////////////////////////////////////////////////////
    void toStringPreOrder(Node* node, stringstream& ss) {
        if (node != nullptr) {
            ss << node->toString() << " ";
            toStringPreOrder(node->firstPtr, ss);
            for (int i = 0; i < node->numEntries; i++) {
                toStringPreOrder(node->entries[i].rightPtr, ss);
            }
        }
    }

    string toStringPreOrder() {
        stringstream ss;
        toStringPreOrder(root, ss);
        return ss.str();
    }
    void testPrintNode(K* keys, D* data, int size) {
        Node node;

        for (int idx = 0; idx < size; idx++) {
            node.entries[idx].key = keys[idx];
            node.entries[idx].data = data[idx];
        }
        node.numEntries = size;
        cout << node.toString() << endl;
    }
    void searchHelper(Node* root, const K& target, Node*& outNodePtr, int& outEntryIdx, bool& found) {
        if (!root) return;

        if (target < root->entries[0].key) searchHelper(root->firstPtr, target, outNodePtr, outEntryIdx, found);
        else {
            //cout << "do day" << endl;
            outEntryIdx = root->numEntries - 1;
            //cout << outEntryIdx << endl;
            while (target < (root->entries[outEntryIdx]).key) outEntryIdx--;

            if (target == root->entries[outEntryIdx].key) {
                found = true;
                outNodePtr = root;
            }
            else {
                searchHelper((root->entries[outEntryIdx]).rightPtr, target, outNodePtr, outEntryIdx, found);
            }
        }
    }

    bool search(const K& key, Node*& outNodePtr, int& outEntryIdx) {
        bool found = false;
        searchHelper(root, key, outNodePtr, outEntryIdx, found);
        return found;
    }
    void insertEntry(Node*& node, int entryNdx, const Entry& newEntry) {
        //cout << "hihi: " << node->numEntries << ", " << entryNdx << endl;

        int shifter = node->numEntries;
        while (shifter > entryNdx) {
            node->entries[shifter] = node->entries[shifter - 1];
            shifter--;
        }
        node->entries[entryNdx] = newEntry;
    }

    void splitNode(Node*& node, int entryNdx, Entry& upEntry) {
        //cout << "\nBat dau goi ham splitNode; upEntry.key: " << upEntry.key << ", upEntry.data: " << upEntry.data << endl;
        // insertEntry(node, entryNdx, upEntry);
        // node->numEntries++;
        // //cout << node->toString() << endl;
        // int pivot = node->numEntries / 2;
        // Node* rightPtr = new Node();
        // rightPtr->numEntries = node->numEntries - pivot - 1;
        // for(int i = pivot + 1; i < node->numEntries; i++) rightPtr->entries[i - pivot - 1] = node->entries[i];
        // node->numEntries = node->numEntries - rightPtr->numEntries - 1;
        // Node* newNode = new Node();
        // newNode->numEntries = node->numEntries;
        // for(int i = 0; i < newNode->numEntries; i++) newNode->entries[i] = node->entries[i];
        // free(node);
        // node = newNode;
        // //cout << node->toString() << endl;
        // upEntry.data = node->entries[pivot].data;
        // upEntry.key = node->entries[pivot].key;
        // upEntry.rightPtr = rightPtr;
        // rightPtr->firstPtr = node->entries[pivot].rightPtr;



        //cout << "\nBat dau goi ham splitNode; upEntry.key: " << upEntry.key << ", upEntry.data: " << upEntry.data << endl;
        //cout << "entryNdx: " << entryNdx << ", node->numEntries: " << node->numEntries << endl;
        int minEntries = (node->numEntries) / 2;
        int fromNdx;
        Node* rightPtr = new Node();

        if (entryNdx <= minEntries) {
            fromNdx = minEntries;
        }
        else {
            fromNdx = minEntries + 1;
        }
        int toNdx = 0;
        rightPtr->numEntries = node->numEntries - fromNdx;
        //cout << "fromNdx = " << fromNdx << ", minEntriens = " << minEntries << endl;
        //cout << "rightPtr->numEntries: " << rightPtr->numEntries << endl;

        while (fromNdx < node->numEntries) {
            rightPtr->entries[toNdx] = node->entries[fromNdx];
            fromNdx++;
            toNdx++;
        }
        node->numEntries -= rightPtr->numEntries;
        if (entryNdx <= minEntries) {
            insertEntry(node, entryNdx, upEntry);
            node->numEntries++;
        }
        else {
            insertEntry(rightPtr, entryNdx - minEntries - 1, upEntry);
            rightPtr->numEntries++;
        }

        int medianNdx = node->numEntries - 1;
        upEntry.data = node->entries[medianNdx].data;
        upEntry.key = node->entries[medianNdx].key;
        //cout << "upEntry.key = " << upEntry.key << endl;
        upEntry.rightPtr = rightPtr;
        rightPtr->firstPtr = node->entries[medianNdx].rightPtr;
        node->numEntries--;
        //cout << node->toString() << endl;
        //cout << rightPtr->toString() << endl;
        //cout << endl;

    }



    int searchNode(Node* nodePtr, K target) {
        if (target < nodePtr->entries[0].key) return 0;
        else {
            int res = nodePtr->numEntries - 1;
            while (target < nodePtr->entries[res].key) res--;
            //if(res < 0) res = 0;
            return res + 1;
        }
    }

    bool insertNode(Node*& root, const K& key, const D& data, Entry& upEntry) {
        bool taller;
        if (!root) {
            cout << "goi ham insertNode: TH root rong" << endl;
            upEntry.data = data;
            upEntry.key = key;
            upEntry.rightPtr = nullptr;
            taller = true;
        }
        else {
            cout << "goi ham insertNode: TH root ko rong" << endl;
            int entryNdx = searchNode(root, key);
            Node* subTree = nullptr;
            if (entryNdx > 0) {
                subTree = root->entries[entryNdx - 1].rightPtr;
                cout << "to here" << endl;
                if (subTree) cout << "xuat subTree: " << subTree->toString() << endl;
            }
            else subTree = root->firstPtr;
            taller = insertNode(subTree, key, data, upEntry);
            if (taller) {
                if (root->isFull()) {
                    cout << "root dang xet dang day" << endl;
                    splitNode(root, entryNdx, upEntry);
                    taller = true;
                }
                else {
                    cout << "root dang xet chua day: upEntry.key: " << upEntry.key << ", upEntry.data: " << upEntry.data << endl;
                    cout << "entryNdx = " << entryNdx << endl;
                    insertEntry(root, entryNdx, upEntry);
                    root->numEntries++;
                    taller = false;
                }
            }
        }
        return taller;
    }

    Node* insertBT(Node*& root, const K& key, const D& data) {
        Entry upEntry;
        bool taller = insertNode(root, key, data, upEntry);
        if (taller) {
            cout << "Truong hop taller: upEntry.key: " << upEntry.key << ", upEntry.data: " << upEntry.data << endl;
            Node* newPtr = new Node();
            newPtr->entries[0] = upEntry;
            newPtr->firstPtr = root;
            newPtr->numEntries = 1;
            root = newPtr;
            //if(root) cout << "sau taller, root ko con rong nua" << endl;
        }
        return root;
    }

    /// TODO: Implement following method
    void insert(const K& key, const D& data) {
        cout << string(10, '~') << "Bat dau insert: " << key << ", " << data << endl;
        if (!root) cout << "trc insert, root dang rong" << endl;

        root = insertBT(root, key, data);

        if (root) {
            cout << "sau insert, root ko con rong nua, xuat ra thong tin: " << endl;
            cout << root->toString() << endl;
        }
        cout << string(50, '-') << endl;
    }

    /// END STUDENT CODE
};

int main() {
    BTree<int, int, 3> bTree;
    //int keys[] = { 9, 21, 14, 12, 23, 45, 56, 17 };
    int keys[] = { 9, 21, 14, 12, 23 };
    int size = sizeof(keys) / sizeof(int);

    for (int idx = 0; idx < size; idx++) {
        bTree.insert(keys[idx], idx);
    }

    cout << bTree.toStringPreOrder() << endl;

    return 0;
}