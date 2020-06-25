#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <regex>
#include <map>
#include <cstdlib>

using namespace std;

class Object{
public:
    Object() = default;
    Object(const Object&) = default;
    virtual ~Object() = default;
    virtual string whatAmI() const = 0;
    virtual string toString() const = 0;
    virtual operator string() const
    {
        return toString();
    }
    virtual Object* clone() const = 0;
    virtual Object& operator+=(const Object&) = 0;
    virtual Object& append(const Object& obj)
    {
        *this += obj;
        return *this;
    }
protected:
    bool CheckIfRightType(const Object& obj){
        return whatAmI() == obj.whatAmI();
    }
};

class StringObject : public Object{
    string text;
public:
    StringObject() = default;
    StringObject(const StringObject& str) = default;
    StringObject(const string &text) : text(text) {}
    ~StringObject(){
        cout << "To ja destruktor StringObject!" << endl;
    }
    string whatAmI() const {
        return "I am a String Object.";
    }
    string toString() const {
        return text;
    }
    Object* clone() const {
        return new StringObject(*this);
    }
    Object& operator+=(const Object& str) override{
        if(CheckIfRightType(str)){
                auto conversion = dynamic_cast<const StringObject*>(&str);
                this->text += conversion->text;
        }
        else{
            cout << "Error - different types of Objects." << endl;
        }
        return *this;
    }
};

class IntObject : public Object{
    int IntValue;
public:
    IntObject() = default;
    IntObject(const IntObject& intt) = default;
    IntObject(const int Value) : IntValue(Value) {};
    ~IntObject(){
        cout << "To ja destruktor IntObject!" << endl;
    }
    string whatAmI() const {
        return "I am an Int Object.";
    }
    string toString() const {
        return std::to_string(IntValue);
    }
    Object* clone() const{
        return new IntObject(*this);
    }
    Object& operator+=(const Object& intt) {
        if(CheckIfRightType(intt)){
            IntValue += dynamic_cast<const IntObject&>(intt).IntValue;
        }
        else{
            cout << "Error - different types of Objects." << endl;
        }
        return *this;
    }
    int GetInt(){
        return this->IntValue;
    }
};

class DoubleObject : public Object{
    double DoubleValue;
public:
    DoubleObject() = default;
    DoubleObject(const DoubleObject& doub) = default;
    DoubleObject(const double Value) : DoubleValue(Value) {};
    ~DoubleObject(){
        cout << "To ja destruktor DoubleObject!" << endl;
    }
    string whatAmI() const override {
        return "I am a Double Object.";
    }
    string toString() const {
        return std::to_string(DoubleValue);
    }
    Object* clone() const {
        return new DoubleObject(*this);
    }
    Object& operator+=(const Object& doub) {
        if(CheckIfRightType(doub)){
            DoubleValue += dynamic_cast<const DoubleObject&>(doub).DoubleValue;
        }
        else{
            cout << "Error - different types of Objects." << endl;
        }
        return *this;
    }
    double GetDouble() {
        return this->DoubleValue;
    }
};

void save2File(const list<Object*>&objects, const string& fileName){
    ofstream file(fileName);

    if(file){
        for(Object* element : objects){ //Iterowanie po wartosciach w liscie
                file << element->toString() << endl;
            }
    }
}

list<DoubleObject*> copyDoubleObjects(const list<DoubleObject*>&objects){
    list<DoubleObject*>objects_copy;
    for(DoubleObject* element : objects){ //Iterowanie po wartosciach w liscie
                objects_copy.push_back(element);
            }
    return objects_copy;
}

list<IntObject*> chooseInts(const list<Object*>&objects)
{
    list<IntObject*> objects_int;

    for (Object* obj : objects)
    {
        if(obj->whatAmI() == "I am an Int Object.")
            objects_int.push_back(dynamic_cast<IntObject*>(obj));
    }

    return objects_int;
}

//LISTA JEDNOKIERUNKOWA:
class List{
public:
    List() : number_of_elements(0), head(nullptr) {}
    List(const List& list_copy) : number_of_elements(list_copy.number_of_elements){
        for(Node* node = head; node; node = node->next){
            if(node->obj){
                auto ObjectCopy = node->obj->clone();
                push_back(ObjectCopy);
            }
        }
    }

    List(List&& List2Move) : number_of_elements(std::move(List2Move.number_of_elements)),
    head(std::move(List2Move.head))
    {
      List2Move.head = nullptr;
      List2Move.number_of_elements = 0;
    }

    void push_back(Object* object){
        Node* newNode = new Node(object);
        if(head){
            temp(number_of_elements-1)->next = newNode;
        }
        else{
            head = newNode;
        }
        number_of_elements++;
    } //dodaje element na koniec

    List& operator+=(Object* object){
        push_back(object);
        return *this;
    } //to samo co wy¿ej

    Object* pop_back(){
        if(number_of_elements<=0){
            cout << "There are no elements to remove.";
        }
        Node* lastNode = temp(number_of_elements-1);
        Object* ValueOfLast = lastNode->obj;
        delete lastNode;
        if(number_of_elements == 1){
            head = nullptr;
        }
        else{
            Node* nodeBefore = temp(number_of_elements-1);
            nodeBefore->next = nullptr;
        }
        --number_of_elements;

        return ValueOfLast;
    } //usuwa ostatni element z listy

    Object* operator-(){
        return pop_back();
    } //to samo co wy¿ej

    Object* at(size_t index) const{
        return temp(index)->obj;
    } //zwraca element o zadanym indeksie

    Object* operator[](size_t index) const{
        return at(index);
    }

    bool operator&&(const Object& obj){
        bool yes_no = false;
        for(size_t i = 0; i<number_of_elements; i++){
            if((*this)[i]->toString() == obj.toString()){
                cout << "wtf" << endl;
                yes_no = true;
            }
        }
        return yes_no;
    }

    Object* operator->(){
        string empty_list="PUSTA LISTA!";
        if(number_of_elements==0) throw empty_list;
        size_t index=static_cast<size_t>((rand() % static_cast<int>(number_of_elements)));
        return (*this)[index];
    }

    size_t size() const{
        return number_of_elements;
    }
    ~List(){
        while(number_of_elements>0){
            pop_back();
        }
    }
private:
    struct Node{
        Object* obj;
        Node* next;

        Node(Object* obj=nullptr): obj(obj), next(nullptr) {}
        Node(const Node&) = delete;
        ~Node(){
            delete obj;
            obj = nullptr;
            next = nullptr;
            //Usuwamy obiekt, ale wskaŸnik wci¹¿ wskazuje na jakiœ adres, dlatego musimy zainicjalizowaæ nullem
        }
    };

    Node* head;
    size_t number_of_elements;

    Node* temp(size_t index) const{
        Node* node = head;
        if(index<=number_of_elements){
            for(size_t i = 0; i<index; i++){
                node = node->next;
            }
            return node;
        }
        else{
            return nullptr;
        }
    }
};

List getElementsPassingPredicate(const List& myList, bool(*predicate)(const Object*)){
    List ret;
    for(size_t i=0; i < myList.size() ; i++){
        if(predicate(myList[i]) == true){
            ret+= myList[i];
        }
    }
    return ret;
}

bool IsMail(const Object* obj){
    const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    return regex_match(obj->toString(), pattern);
}

struct Predicate{
    virtual bool operator()(const Object* obj) const = 0;
    virtual ~Predicate(){}
};

struct IsMail : Predicate{
    bool operator()(const Object* obj) const override{
        const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
        return regex_match(obj->toString(), pattern);
    }
};

size_t countElementsPassingPredicate(const List& myList, const Predicate& predicate){
    size_t how_many = 0;
    for(size_t i=0; i < myList.size() ; i++){
        if(predicate(myList[i]) == true){
            how_many++;
        }
        return how_many;
    }

}

struct IsGreaterThan{
private:
    int number;
public:
    IsGreaterThan(int numb) : number(numb) {}
    bool operator()(Object* obj) const{
        if(obj->whatAmI() == "I am a String Object."){
            return false;
        }
        if(obj->whatAmI() == "I am an Int Object."){
            IntObject* object = dynamic_cast<IntObject*>(obj);
            return object->GetInt() > number;
        }
        if(obj->whatAmI() == "I am a Double Object."){
            DoubleObject* object = dynamic_cast<DoubleObject*>(obj);
            return object->GetDouble() > number;
        }
    }
};

int main()
{
    cout << "CATCH ME IF U CAN!" << endl;
    List lista2;
    try{
    cout<<lista2->toString()<<endl;
    }
    catch (string x){
        cout<<"Oh.. You dit it!: "<<x<<endl;
    }

    List lista1;
    StringObject s1("ania.gn@interia.pl"), s2("xxx.xxx@xxx.xxx");
    IntObject i1(5);
    DoubleObject d1(3.6432);
    lista1.push_back(&i1);
    lista1.push_back(&d1);
    lista1.push_back(&s1);
    lista1.push_back(&s2);

    cout << "List before getElementsPassingPredicate: " << lista1.size() << endl;
    for(size_t i=0; i< lista1.size(); i++){
    cout<<lista1[i]->toString()<<endl;
    }
    List lista3 = getElementsPassingPredicate(lista1, IsMail);
    cout << "List after getElementPassingPredicate: " << lista3.size() << endl;
    for(size_t i=0; i< lista3.size(); i++){
    cout<<lista3[i]->toString()<<endl;
    }

    return 0;
}
