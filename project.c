#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdbool.h>
#define MAX_LOCATIONS 100
#define MAX_LOGIN_ATTEMPTS 3

struct Location {
    char name[50];
    int weight;
    int index;
};

struct Graph {
    int V; // Number of locations (vertices)
    int adjacencyMatrix[MAX_LOCATIONS][MAX_LOCATIONS];
    struct Location locations[MAX_LOCATIONS];
};

typedef struct User {
    char username[30];
    char password[30];
    char passwordHint[50];
    int isAdmin;
} User;

User users[] = {
    {"admin", "admin123", "Admin's password", 1},
    {"user", "password", "User's password", 0},
};

int numUsers = sizeof(users) / sizeof(users[0]);

// Function prototypes
void saveGraphToFile(struct Graph *graph);
void loadGraphFromFile(struct Graph *graph);
void addLocation(struct Graph *graph, const char *name, int weight);
void addEdge(struct Graph *graph, int source, int destination, int weight);
void displayGraph(struct Graph *graph);
int findLocationIndex(struct Graph *graph, const char *name);
void findShortestPathDijkstra(struct Graph *graph, int start, int destination);
void findShortestPathBellmanFord(struct Graph *graph, int start, int destination);
void findShortestPathBFS(struct Graph *graph, int start, int destination);
void userMenu(struct Graph *graph, User loggedInUser);

// ... (Previous code)

int authenticateUser(char username[], char password[], User *loggedInUser) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            *loggedInUser = users[i];
            return 1; // Authentication successful
        }
    }
    return 0; // Authentication failed
}

void login(User *loggedInUser) {
    char username[30], password[30];
    int loginAttempts = 0;

    while (loginAttempts < MAX_LOGIN_ATTEMPTS) {
        printf("\nEnter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);

        if (authenticateUser(username, password, loggedInUser)) {
            printf("\nLogin successful!\n");
            return;
        } else {
            printf("\nIncorrect username or password. Please try again.\n");
            loginAttempts++;
        }
    }

    printf("\nToo many incorrect login attempts. Exiting program.\n");
    exit(1);
}

int main() {
    struct Graph graph;
    graph.V = 0;

    // Load graph data from file (if available)
    loadGraphFromFile(&graph);

    User loggedInUser;
    login(&loggedInUser);

    int choice;
    do {
        printf("\nMain Menu\n");
        printf("1. Admin\n");
        printf("2. User\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // Admin Menu
                if (loggedInUser.isAdmin) {
                    do {
                        printf("\nAdmin Menu\n");
                        printf("1. Add Location\n");
                        printf("2. Add Edge\n");
                        printf("3. Back to Main Menu\n");
                        printf("Enter your choice: ");
                        scanf("%d", &choice);

                        switch (choice) {
                            case 1:
                                // Add Location
                                {
                                    char name[50];
                                    int weight;
                                    printf("Enter location name: ");
                                    scanf("%s", name);
                                    printf("Enter weight for %s: ", name);
                                    scanf("%d", &weight);
                                    addLocation(&graph, name, weight);
                                    printf("Location %s added successfully!\n", name);
                                }
                                break;
                            case 2:
                                // Add Edge
                                {
                                    int source, destination, weight;
                                    printf("Enter source location index (0 to %d): ", graph.V - 1);
                                    scanf("%d", &source);
                                    printf("Enter destination location index (0 to %d): ", graph.V - 1);
                                    scanf("%d", &destination);
                                    printf("Enter weight for the edge: ");
                                    scanf("%d", &weight);
                                    addEdge(&graph, source, destination, weight);
                                    printf("Edge added successfully!\n");
                                }
                                break;
                            case 3:
                                // Back to Main Menu
                                break;
                            default:
                                printf("Invalid choice. Please enter a valid option.\n");
                        }
                    } while (choice != 3);

                    // Save graph data to file
                    saveGraphToFile(&graph);
                } else {
                    printf("Access denied. You are not an admin.\n");
                }
                break;

            case 2:
                // User Menu
              userMenu(&graph, loggedInUser);
                break;
            case 3:
                // Exit
                break;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 3);

    return 0;
}

void saveGraphToFile(struct Graph *graph) {
    FILE *file = fopen("graph_data.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Write number of locations to the file
    fprintf(file, "%d\n", graph->V);

    // Write location names, weights, and indices to the file
    for (int i = 0; i < graph->V; i++) {
        fprintf(file, "%s %d %d\n", graph->locations[i].name, graph->locations[i].weight, graph->locations[i].index);
    }

    // Write adjacency matrix to the file
    for (int i = 0; i < graph->V; i++) {
        for (int j = 0; j < graph->V; j++) {
            fprintf(file, "%d ", graph->adjacencyMatrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Graph data saved to file.\n");
}

void loadGraphFromFile(struct Graph *graph) {
    FILE *file = fopen("graph_data.txt", "r");
    if (file == NULL) {
        printf("No existing graph data found.\n");
        return;
    }

    // Read number of locations from the file
    fscanf(file, "%d", &(graph->V));

    // Read location names, weights, and indices from the file
    for (int i = 0; i < graph->V; i++) {
        fscanf(file, "%s %d %d", graph->locations[i].name, &(graph->locations[i].weight), &(graph->locations[i].index));
    }

    // Read adjacency matrix from the file
    for (int i = 0; i < graph->V; i++) {
        for (int j = 0; j < graph->V; j++) {
            fscanf(file, "%d", &(graph->adjacencyMatrix[i][j]));
        }
    }

    fclose(file);
    printf("Graph data loaded from file.\n");
}

/*void addLocation(struct Graph *graph, const char *name, int weight) {
    if (graph->V < MAX_LOCATIONS) {
        strcpy(graph->locations[graph->V].name, name);
        graph->locations[graph->V].weight = weight;
        graph->locations[graph->V].index = graph->V;

        // Initialize adjacency matrix values for the new location
        for (int i = 0; i <= graph->V; i++) {
            graph->adjacencyMatrix[graph->V][i] = 0;
            graph->adjacencyMatrix[i][graph->V] = 0;
        }

        graph->V++;
    } else {
        printf("Maximum number of locations reached. Cannot add more.\n");
    }
}

void addEdge(struct Graph *graph, int source, int destination, int weight) {
    if (source >= 0 && source < graph->V && destination >= 0 && destination < graph->V) {
        graph->adjacencyMatrix[source][destination] = weight;
        graph->adjacencyMatrix[destination][source] = weight;  // Add the reverse edge for an undirected graph
    } else {
        printf("Invalid source or destination index. Please enter valid indices.\n");
    }
}
*/
void addLocation(struct Graph *graph, const char *name, int weight) {
    if (graph->V < MAX_LOCATIONS) {
        int index = findLocationIndex(graph, name);

        if (index == -1) {
            // Location does not exist, add it
            strcpy(graph->locations[graph->V].name, name);
            graph->locations[graph->V].weight = weight;
            graph->locations[graph->V].index = graph->V;

            // Initialize adjacency matrix values for the new location
            for (int i = 0; i <= graph->V; i++) {
                graph->adjacencyMatrix[graph->V][i] = 0;
                graph->adjacencyMatrix[i][graph->V] = 0;
            }

            graph->V++;
        } else {
            // Location already exists, update its weight
            printf("Location %s already exists. Updating weight to %d.\n", name, weight);
            graph->locations[index].weight = weight;
        }
    } else {
        printf("Maximum number of locations reached. Cannot add more.\n");
    }
}

void addEdge(struct Graph *graph, int source, int destination, int weight) {
    if (source >= 0 && source < graph->V && destination >= 0 && destination < graph->V) {
        // Check if edge already exists
        if (graph->adjacencyMatrix[source][destination] == 0) {
            // Edge does not exist, add it
            graph->adjacencyMatrix[source][destination] = weight;
            graph->adjacencyMatrix[destination][source] = weight;  // Add the reverse edge for an undirected graph
            printf("Edge between %s and %s added successfully!\n", graph->locations[source].name, graph->locations[destination].name);
        } else {
            printf("Edge between %s and %s already exists. Updating weight to %d.\n", graph->locations[source].name, graph->locations[destination].name, weight);
            // Edge already exists, update its weight
            graph->adjacencyMatrix[source][destination] = weight;
            graph->adjacencyMatrix[destination][source] = weight;
        }
    } else {
        printf("Invalid source or destination index. Please enter valid indices.\n");
    }
}
void printPath(struct Graph *graph, int start, int destination, int parent[]) {
    if (destination == start) {
        printf("%s", graph->locations[start].name);
        return;
    }

    printPath(graph, start, parent[destination], parent);

    printf(" -> %s", graph->locations[destination].name);
}

void findShortestPathDijkstra(struct Graph *graph, int start, int destination) {
    // Dijkstra's Algorithm
    int distance[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS] = {0};
    int parent[MAX_LOCATIONS];  // To store the parent of each vertex in the path

    for (int i = 0; i < graph->V; i++) {
        distance[i] = INT_MAX;
    }

    distance[start] = 0;
    parent[start] = -1;

    for (int count = 0; count < graph->V - 1; count++) {
        int minDist = INT_MAX, minIndex;

        for (int v = 0; v < graph->V; v++) {
            if (!visited[v] && distance[v] < minDist) {
                minDist = distance[v];
                minIndex = v;
            }
        }

        visited[minIndex] = 1;

        for (int v = 0; v < graph->V; v++) {
            if (!visited[v] && graph->adjacencyMatrix[minIndex][v] &&
                distance[minIndex] != INT_MAX &&
                distance[minIndex] + graph->adjacencyMatrix[minIndex][v] < distance[v]) {
                distance[v] = distance[minIndex] + graph->adjacencyMatrix[minIndex][v];
                parent[v] = minIndex;
            }
        }
    }

    printf("Dijkstra's Algorithm Result:\n");
    printf("Shortest distance from %s to %s: %d\n", graph->locations[start].name, graph->locations[destination].name, distance[destination]);

    printf("Shortest path: ");
    printPath(graph, start, destination, parent);
    printf("\n");
}

void findShortestPathBellmanFord(struct Graph *graph, int start, int destination) {
    // Bellman-Ford Algorithm
    int distance[MAX_LOCATIONS];
    int parent[MAX_LOCATIONS];  // To store the parent of each vertex in the path

    for (int i = 0; i < graph->V; i++) {
        distance[i] = INT_MAX;
    }
    distance[start] = 0;
    parent[start] = -1;

    for (int i = 0; i < graph->V - 1; i++) {
        for (int u = 0; u < graph->V; u++) {
            for (int v = 0; v < graph->V; v++) {
                if (graph->adjacencyMatrix[u][v] &&
                    distance[u] != INT_MAX &&
                    distance[u] + graph->adjacencyMatrix[u][v] < distance[v]) {
                    distance[v] = distance[u] + graph->adjacencyMatrix[u][v];
                    parent[v] = u;
                }
            }
        }
    }

    // Check for negative weight cycles
    for (int u = 0; u < graph->V; u++) {
        for (int v = 0; v < graph->V; v++) {
            if (graph->adjacencyMatrix[u][v] &&
                distance[u] != INT_MAX &&
                distance[u] + graph->adjacencyMatrix[u][v] < distance[v]) {
                printf("Graph contains a negative weight cycle. Bellman-Ford cannot find the shortest paths.\n");
                return;
            }
        }
    }

    printf("Bellman-Ford Algorithm Result:\n");
    printf("Shortest distance from %s to %s: %d\n", graph->locations[start].name, graph->locations[destination].name, distance[destination]);

    printf("Shortest path: ");
    printPath(graph, start, destination, parent);
    printf("\n");
}

void findShortestPathBFS(struct Graph *graph, int start, int destination) {
    // Breadth-First Search (BFS)
    bool visited[MAX_LOCATIONS] = {false};
    int queue[MAX_LOCATIONS];
    int front = -1, rear = -1;

    visited[start] = true;
    queue[++rear] = start;

    printf("Breadth-First Search (BFS) Result:\n");

    while (front != rear) {
        int current = queue[++front];
        printf("%s -> ", graph->locations[current].name);

        if (current == destination) {
            printf("\nDestination reached.\n");
            return;
        }

        for (int i = 0; i < graph->V; i++) {
            if (graph->adjacencyMatrix[current][i] && !visited[i]) {
                visited[i] = true;
                queue[++rear] = i;
            }
        }
    }

    printf("\n");
}

int findLocationIndex(struct Graph *graph, const char *name) {
    for (int i = 0; i < graph->V; i++) {
        if (strcmp(graph->locations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

void userMenu(struct Graph *graph, User loggedInUser) {
    // Rest of the userMenu implementation remains the same
    char startLocation[50], destLocation[50];

    printf("\nEnter start location: ");
    scanf("%s", startLocation);

    printf("Enter destination location: ");
    scanf("%s", destLocation);
    // Find the indices corresponding to the entered location names
    int start = findLocationIndex(graph, startLocation);
    int destination = findLocationIndex(graph, destLocation);

    // Check if the entered location names are valid
    if (start == -1 || destination == -1) {
        printf("Invalid start or destination location names. Please enter valid location names.\n");
        return;
    }
    printf("%d", start);
    printf("%d", destination);

    // Call Dijkstra's Algorithm
    findShortestPathDijkstra(graph, start, destination);

    // Call Bellman-Ford Algorithm
    findShortestPathBellmanFord(graph, start, destination);

    // Call BFS Algorithm
    findShortestPathBFS(graph, start, destination);
}