#include <iostream>
using namespace std;
#include <list>
#include <stack>
#include <deque>
#include <set>  
#include <queue>
#include <cstdlib>  
#include <ctime> 

struct Node {
    int id;
    float x, y;
    int hScore;
    Node(int _id, float _x, float _y){
        id =_id;
        x = _x;
        y = _y;
    }
    bool operator>(Node& other)  {
        return hScore > other.hScore;
    }
};

struct Edge {
    int origen, destino;
    Edge(int n1, int n2){
        if (n1 < n2) { 
            origen = n1; destino = n2;
        } else {
            origen = n2; destino = n1;
        }
    }
};

class Grafo{
    public:
    Node* head = nullptr;
    int filas;
    int col;
    int tam_cuadrado;
    vector<Node*> nodos;
    list<Edge> edges;
    vector<vector<Node*>> adyacencia;
    Grafo(int f, int c, int t){
        filas = f;
        col = c;
        tam_cuadrado = t;
    }
    void generarGrafo();
    void eliminarEdges(int porcentaje);
    vector<vector<Node*>> listaAdyacencia();
    tuple<deque<Node*>, vector<bool>, vector<int>> busqueda_profundidad(Node* ini, Node* obj);
    tuple<deque<Node*>, vector<bool>, vector<int>> busqueda_amplitud(Node* ini, Node* obj);
    tuple<deque<Node*>, vector<bool>, vector<int>> busqueda_A(Node* ini, Node* obj);
    tuple<deque<Node*>, vector<bool>, vector<int>> busqueda_greedy_bfs(Node* ini, Node* obj);
    int heuristica (Node* A, Node* B);

        
};



void Grafo::generarGrafo(){
    
    // Creando nodos con sus coordenadas
    int id = 0;
    for (int i= 0; i < filas; i++){
        for (int j = 0; j < col; j++){
            float x = j * tam_cuadrado;
            float y = i * tam_cuadrado;
            Node* nuevo = new Node(id,x,y);
            nodos.push_back(nuevo);

            if(!head){
                head = nuevo;
            }
            id++;
        }
    }
    // Conectar edges 

    for (int i= 0; i < filas-1; i++){
        for (int j = 0; j < col-1; j++){
            int nodo_sup_iz = i * col + j;
            int nodo_sup_der = nodo_sup_iz + 1;
            int nodo_inf_iz = (i + 1) * col + j;
            int nodo_inf_der = nodo_inf_iz + 1;

            edges.emplace_back(nodo_sup_iz, nodo_sup_der); 
            edges.emplace_back(nodo_sup_iz, nodo_inf_iz); 
            edges.emplace_back(nodo_inf_iz, nodo_inf_der); 
            edges.emplace_back(nodo_sup_der, nodo_inf_der); 

            edges.emplace_back(nodo_inf_iz, nodo_sup_der);
            edges.emplace_back(nodo_sup_iz, nodo_inf_der);
        }
    }
    eliminarEdges(30);

}


void  Grafo::eliminarEdges(int porcentaje) {
    if (edges.empty()) return;

    int cantidad_borrar = edges.size() * porcentaje / 100;

    for (int i = 0; i < cantidad_borrar; i++) {
        int indice = rand() % edges.size();
        auto it = edges.begin();
        advance(it, indice);
        edges.erase(it);
    }
}

vector<vector<Node*>> Grafo::listaAdyacencia() {
    int n = nodos.size();
    vector<set<Node*>> adj_temp(n);

    for (auto& e : edges) {
        adj_temp[e.origen].insert(nodos[e.destino]);
        adj_temp[e.destino].insert(nodos[e.origen]);
    }
    vector<vector<Node*>> adj(n);
    for (int i = 0; i < n; i++) {
        adj[i] = vector<Node*>(adj_temp[i].begin(), adj_temp[i].end());
    }
    adyacencia = adj;
    return adj;
}

tuple<deque<Node*>, vector<bool>, vector<int>> Grafo::busqueda_profundidad(Node* ini, Node* obj){
    deque<Node*> path;
    int cant_nodos = nodos.size();
    vector<bool> visited(cant_nodos,false);
    vector<int> prev(cant_nodos, -1);

    path.push_back(nodos[ini->id]);
    visited[ini->id] = true;

    while(!path.empty()){
        Node* top = path.back();
        if(top->id == obj->id) break;

        bool avanzamos = false;
        for (Node* vecino : adyacencia[top->id]) {
            if (!visited[vecino->id]) {
                visited[vecino->id] = true;
                prev[vecino->id] = top->id;
                path.push_back(vecino);
                avanzamos = true;
                break; 
            }
        }
        if (!avanzamos) path.pop_back();
    }
    return {path, visited, prev};
}

tuple<deque<Node*>, vector<bool>, vector<int>> Grafo::busqueda_amplitud(Node* ini, Node* obj) {
    deque<Node*> path;
    int cant_nodos = nodos.size();
    vector<bool> visited(cant_nodos, false);
    vector<int> prev(cant_nodos, -1);

    deque<Node*> queue;
    queue.push_back(nodos[ini->id]);
    visited[ini->id] = true;

    while (!queue.empty()) {
        Node* actual = queue.front();
        queue.pop_front();

        if (actual->id == obj->id) {
            break;
        }

        for (Node* vecino : adyacencia[actual->id]) {
            if (!visited[vecino->id]) {
                visited[vecino->id] = true;
                prev[vecino->id] = actual->id;
                queue.push_back(vecino);
            }
        }
    }

    if (prev[obj->id] == -1 && obj->id != ini->id) {
        return {path, visited, prev}; 
    }

    for (int at = obj->id; at != -1; at = prev[at]) {
        path.push_front(nodos[at]);
    }

    return {path, visited, prev};
}

int Grafo::heuristica (Node* A, Node* B){
    return sqrt(pow(A->x - B->x,2) + pow(A->y - B->y,2));
}

tuple<deque<Node*>, vector<bool>, vector<int>> Grafo::busqueda_A(Node* ini, Node* obj){
    int cant_nodos = nodos.size();
    vector<int> gScore(cant_nodos, std::numeric_limits<int>::max());
    vector<int> tScore(cant_nodos, std::numeric_limits<int>::max());
    vector<int> prev_path(cant_nodos, -1);
    vector<bool> visited (cant_nodos, false);

    gScore[ini->id] = 0;
    tScore[ini->id] = heuristica(ini,obj);

    priority_queue<pair<int, Node*>, vector<pair<int, Node*>>, greater<pair<int, Node*>>> queue;
    queue.push({tScore[ini->id],ini});

    while(!queue.empty()){
        Node* current = queue.top().second;
        queue.pop();

        if(visited[current->id]) continue;
        visited[current->id] = true;

        if(current->id == obj->id){
            deque<Node*> path;
            for(int at = obj->id; at != -1; at = prev_path[at]){
                path.push_front(nodos[at]);
            }
            return {path, visited, prev_path};
        }
        
        for(Node* vecino: adyacencia[current->id]){
            if(visited[vecino->id]) continue;

            int possible_g = gScore[current->id] + 1;//posiblemente tam_cuadrado sea 1
            if(possible_g < gScore[vecino->id]){
                prev_path[vecino->id] = current->id;
                gScore[vecino->id] = possible_g;
                tScore[vecino->id] =  possible_g + heuristica(vecino,obj);
                queue.push({tScore[vecino->id], vecino});

            }
        }
    }
    return {{}, visited, prev_path};
}

tuple<deque<Node*>, vector<bool>, vector<int>> Grafo::busqueda_greedy_bfs(Node* ini, Node* obj){
    int cant_nodos = nodos.size();
    vector<int> hScore(cant_nodos, std::numeric_limits<int>::max());
    vector<int> prev_path(cant_nodos, -1);
    vector<bool> visited (cant_nodos, false);

    hScore[ini->id] = heuristica(ini,obj);

    priority_queue<pair<int, Node*>, vector<pair<int, Node*>>, greater<pair<int, Node*>>> queue;
    queue.push({hScore[ini->id],ini});

    while(!queue.empty()){
        Node* current = queue.top().second;
        queue.pop();

        if(visited[current->id]) continue;
        visited[current->id] = true;

        if(current->id == obj->id){
            deque<Node*> path;
            for(int at = obj->id; at != -1; at = prev_path[at]){
                path.push_front(nodos[at]);
            }
            return {path, visited, prev_path};
        }
        
        for(Node* vecino: adyacencia[current->id]){
            if(visited[vecino->id]) continue;
            prev_path[vecino->id] = current->id;
            hScore[vecino->id] =  heuristica(vecino,obj);
            queue.push({hScore[vecino->id],vecino});
        }
    }
    return {{}, visited, prev_path};
}

/*
int main(){
    int inicio;
    int objetivo;
    Grafo grafo(4,4,3);
    
    
    //srand(time(0));
    grafo.generarGrafo(4,4,3); //elimina el 30% de edges
    grafo.listaAdyacencia();

    deque<int> resultado  = grafo.busqueda_profundidad(2, 10);//nodo inicio, nodo obj
    
    cout<<"Path DFS ";
    for(auto i: resultado){
        cout<<i<<" ";
    }

}*/