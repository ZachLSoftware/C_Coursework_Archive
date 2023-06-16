#include "HybridTable.h"
#include <iostream>

Node::Node(int index, int val) {
	index_=index;
	val_=val;
	next_=nullptr;
}

Node::Node(int index, int val, Node* next) {
	index_=index;
	val_=val;
	next_=next;
}

Node::~Node() {
	//When node is deleted, points next_ to null.
	next_=nullptr;
}

HybridTable::HybridTable() {
	//Intializes a new array and initialize to 0.
	array_ = new int[INITIAL_ARRAY_SIZE]{};

	//Set array and total size private variables
	arrSize_=INITIAL_ARRAY_SIZE;
	totalSize_=INITIAL_ARRAY_SIZE;

	//sets list_ variable to nullptr.
	list_=nullptr;
}

//Create new array to size n and intialize with given array
//Set list_ to null and set sizes
HybridTable::HybridTable(const int *p, int n) {
	array_=new int[n];
	for(int i=0; i<n; i++){
		array_[i]=p[i];
	}

	//Initialize other attributes
	list_=nullptr;
	arrSize_=n;
	totalSize_=n;
}

HybridTable::~HybridTable() {
	//Delete the array to free memory
	delete [] array_;

	//Traverse the list and delete each node.
	while(list_!=nullptr){
		Node* temp=list_;
		list_=list_->next_;
		delete temp;
	}
}

HybridTable::HybridTable(const HybridTable& other) {

	//Create a new array using the size from given Hybrid table and copy values
	array_=new int[other.arrSize_];
	for(int i=0;i<other.arrSize_;i++){
		array_[i]=other.array_[i];
	}

	//Set array and total size from given Hybrid Table
	arrSize_=other.arrSize_;
	totalSize_=other.totalSize_;

	//If given table has no list, set list_ to nullptr
	if(other.list_==nullptr) list_=nullptr;
	else{
		//Create the head
		//Set previous node to create links
		//Move to next node
		Node* tmp=other.list_;
		Node* tmpList=new Node(tmp->index_, tmp->val_);
		Node* prev=tmpList;
		tmp=tmp->next_;

		//Begin copy
		while(tmp!=nullptr){
			//Make a new node using copied lists index and values, insert and move to next node.
			Node* newNode=new Node(tmp->index_, tmp->val_);
			tmp=tmp->next_;
			prev->next_=newNode;
			prev=newNode;
		}

		//Cleanup the tmp pointers and set list_
		delete tmp;
		list_=tmpList;
		tmpList=nullptr;
	}
}

//Copy Assignment
HybridTable& HybridTable::operator=(const HybridTable& other) {

	//Check if object being copied is the same.
	if(this==&other){
		return *this;
	}
	
	//Create a temporary array to copy the array into
	int* tempArr=new int[other.arrSize_];
	for(int i=0; i<other.arrSize_; i++){
		tempArr[i]=other.array_[i];
	}

	//set array and total size
	arrSize_=other.arrSize_;
	totalSize_=other.totalSize_;

	//Delete the current array_ and set new array
	delete[] array_;
	array_=tempArr;

	//If other list is empty, delete current list and then set list_ to nullptr
	if(other.list_==nullptr){ 

		//Release existing memory
		while(list_!=nullptr){
			Node* temp=list_;
			list_=list_->next_;
			delete temp;
		}
		list_=nullptr;
	}

	//Copy list if not empty
	//Copy other list ptr for traversal, Create new head, set prev for attaching links,
	//and traverse the list.
	else{
		Node* tmp=other.list_;
		Node* tmpList=new Node(tmp->index_, tmp->val_);
		Node* prev=tmpList;
		tmp=tmp->next_;

		//Copy rest of list
		while(tmp!=nullptr){

			//Create new node, add to list, and everything to next node in list.
			Node* next=new Node(tmp->index_, tmp->val_);
			tmp=tmp->next_;
			prev->next_=next;
			prev=next;
		}
		delete tmp;

		//Release existing memory from old list
		while(list_!=nullptr){
			Node* temp=list_;
			list_=list_->next_;
			delete temp;
		}

		//Set list and cleanup ptr
		list_=tmpList;
		tmpList=nullptr;
	}
	return *this;
}

int HybridTable::get(int i) const {

	//If in the array, simply return value
	if(i<=arrSize_-1&&i>=0){
		return array_[i];
	}
	
	//Traverse list. If empty or not in list, return 0
	else{
		if(list_==nullptr) return 0;
		else if(i>=list_->index_){
			Node* tmp = list_;
			while(tmp!=nullptr && i!=tmp->index_){
				tmp=tmp->next_;
			}
			if(tmp==nullptr) return 0;
			else return tmp->val_;
		}
	}
	return 0; //Just in case
}

void HybridTable::set(int i, int val) {

	//Check if set is within the array.
	if(i<arrSize_&&i>=0){
		array_[i]=val;
		return;
	}
	else{
		//create a flag to skip setting if already set
		bool isSet = false;

		//If the list is empty, create new node and place at head
		if(list_==nullptr){
			list_ = new Node(i,val);
			list_->next_=nullptr;
			totalSize_+=1;
			isSet=true;
		}

		//Check if the index is lower then the head index.  
		//Insert before head and make it the new head.
		else if(i<list_->index_){
			Node* node = new Node(i,val);
			node->next_=list_;
			list_=node;
			totalSize_+=1;

			//clean up
			node=nullptr;
			isSet=true;
		}

		/*Begin list insert and check if resize is necessary.
		  Set nodes for traversal and setting.
		  Set count and new size for resize testing.*/
		Node* tmp=list_;
		Node* prev;

		double count=arrSize_;
		int newSize=INITIAL_ARRAY_SIZE;

		//Ensure newsize is next power of 2
		//nextSize grows with search.
		while(newSize<=arrSize_) newSize*=2;
		double nextSize=newSize;
		bool resize = false;

		//Traverse the list (Only break when at the end and the new value has been set)
		while(tmp!=nullptr || !isSet){
		
			//If set, skip to resize test
			if(!isSet){
				if(tmp!=nullptr && i==tmp->index_){
					tmp->val_=val;
					isSet=true;
				}

				//Create new node at end of list
				else if(tmp==nullptr || i<tmp->index_){
					Node* node = new Node(i,val);
					prev->next_=node;
					node->next_=tmp;
					totalSize_+=1;
					isSet=true;

					//Reset tmp to node to ensure accurate counting in Resize Test
					tmp=node;
				}
			}
			
			//Update nextSize to closest power of 2 of current index.
			if(tmp!= nullptr && nextSize<=tmp->index_){
				while(nextSize<=tmp->index_) nextSize*=2;
			}

			//Check if index is in range of a new array size.
			if(tmp!=nullptr && (tmp->index_>=0 && tmp->index_<nextSize)) count++;

			//Test if resize is required.
			if(count/nextSize>=.75){
				resize = true;
				newSize=nextSize;
			}

			//Move to next node
			prev=tmp;
			tmp=tmp->next_;
		}
	
		/*Begin array resize:
		  Create a new array and fill.
		  Traverse existing list and move values to new array as necesary
		  Set new size attributes*/
		if(resize){

			//Create a new array for the resize and fill with current values
			int* newArr=new int[newSize]{};

			for(int j=0; j<arrSize_; j++){
				newArr[j]=array_[j];
			}

			//Cleanup array_, point to new array, update size
			delete[] array_;
			array_=newArr;
			totalSize_=(totalSize_-arrSize_)+newSize;
			arrSize_=newSize;

			Node* node = list_;
			Node* prev;
			
			//Traverse until we've gone of the range of the array or hit end of list.
			while(node!=nullptr && node->index_<arrSize_){

				//If nodes index can fit into the new array, move it to the array and delete
				if(node->index_>0 && node->index_<arrSize_){

					//If node is head
					if(node==list_){
						array_[node->index_]=node->val_;
						list_=list_->next_;
						delete node;
						node=list_;
					}
					else{
						//previous node bypasses current node
						prev->next_=node->next_;
						array_[node->index_]=node->val_;

						//cleanup node and get next node.
						delete node;
						node=prev->next_;
					}
					totalSize_--;
				}

				//If not in array, move to next
				else{
					prev=node;
					node=node->next_;
				}
			}
		}
	}
}

string HybridTable::toString() const {
	string str;

	//Traverse array and add to string
	for(int i=0; i<arrSize_; i++){
		str += std::to_string(i)+ " : " + std::to_string(array_[i]);
		if(i!=arrSize_-1) str+="\n";
	}
	
	//If list is not empty add dashed line and list contents.
	if(list_!=nullptr){
		str += "\n---\n";
		Node* tmp = list_;
		while(tmp != nullptr){
			str+=std::to_string(tmp->index_) + " : " + std::to_string(tmp->val_);

			//Add Next arrow while next is not null
			if(tmp->next_!=nullptr) {str+= " --> ";}
			tmp=tmp->next_;
		}
	}
	return str;
}

int HybridTable::getArraySize() const {
	return arrSize_;
}

int HybridTable::getTotalSize() const {
	return totalSize_;
}