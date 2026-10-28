#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOCATIONS 5

typedef struct Vehicle {
    int vehicleID;
    char numberPlate[30];
    char vehicleType[40];
    float capacity;
    struct Vehicle *left, *right;
} Vehicle;

typedef struct Trip {
    int tripID, vehicleID, source, destination;
    float load;
    struct Trip *next;
} Trip;

char locations[MAX_LOCATIONS][40] = {
    "Jagadevi Quarry", "Krishnagiri Hub", "Chennai Yard",
    "Bengaluru Yard", "Hosur Depot"
};

int graph[MAX_LOCATIONS][MAX_LOCATIONS] = {
    {0,1,0,0,0},
    {1,0,1,1,1},
    {0,1,0,0,0},
    {0,1,0,0,1},
    {0,1,0,1,0}
};

Vehicle *root = NULL;
Trip *tripHead = NULL;

void readLine(char text[], int size) {
    if (fgets(text, size, stdin))
        text[strcspn(text, "\n")] = '\0';
    else
        text[0] = '\0';
}

int readInt(const char prompt[]) {
    char line[100], extra;
    int value;
    while (1) {
        printf("%s", prompt);
        readLine(line, sizeof(line));
        if (sscanf(line, " %d %c", &value, &extra) == 1)
            return value;
        printf("Invalid input! Enter an integer.\n");
    }
}

float readFloat(const char prompt[]) {
    char line[100], extra;
    float value;
    while (1) {
        printf("%s", prompt);
        readLine(line, sizeof(line));
        if (sscanf(line, " %f %c", &value, &extra) == 1)
            return value;
        printf("Invalid input! Enter a number.\n");
    }
}

/* BINARY SEARCH TREE: VEHICLE MANAGEMENT */

Vehicle *createVehicle(int id, const char plate[],
                       const char type[], float capacity) {
    Vehicle *v = malloc(sizeof(Vehicle));
    if (!v) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    v->vehicleID = id;
    snprintf(v->numberPlate, sizeof(v->numberPlate), "%s", plate);
    snprintf(v->vehicleType, sizeof(v->vehicleType), "%s", type);
    v->capacity = capacity;
    v->left = v->right = NULL;
    return v;
}

Vehicle *insertVehicle(Vehicle *node, Vehicle *newVehicle) {
    if (!node) return newVehicle;
    if (newVehicle->vehicleID < node->vehicleID)
        node->left = insertVehicle(node->left, newVehicle);
    else
        node->right = insertVehicle(node->right, newVehicle);
    return node;
}

Vehicle *searchVehicle(Vehicle *node, int id) {
    if (!node || node->vehicleID == id) return node;
    if (id < node->vehicleID) return searchVehicle(node->left, id);
    return searchVehicle(node->right, id);
}

void displayVehicles(Vehicle *node) {
    if (!node) return;
    displayVehicles(node->left);
    printf("%-10d %-18s %-20s %.2f tons\n",
           node->vehicleID, node->numberPlate,
           node->vehicleType, node->capacity);
    displayVehicles(node->right);
}

void addVehicle(void) {
    int id;
    char plate[30], type[40];
    float capacity;

    id = readInt("Enter Vehicle ID: ");
    if (id <= 0) {
        printf("Vehicle ID must be positive.\n");
        return;
    }
    if (searchVehicle(root, id)) {
        printf("Vehicle ID already exists!\n");
        return;
    }

    printf("Enter Vehicle Number Plate: ");
    readLine(plate, sizeof(plate));
    printf("Enter Vehicle Type (Truck/Lorry): ");
    readLine(type, sizeof(type));
    capacity = readFloat("Enter Load Capacity in tons: ");

    if (!plate[0] || !type[0] || capacity <= 0) {
        printf("Invalid vehicle details!\n");
        return;
    }

    root = insertVehicle(root, createVehicle(id, plate, type, capacity));
    printf("Vehicle added successfully!\n");
}

void searchVehicleMenu(void) {
    int id = readInt("Enter Vehicle ID to search: ");
    Vehicle *v = searchVehicle(root, id);
    if (!v) {
        printf("Vehicle not found!\n");
    } else {
        printf("\nVehicle Found!\nID: %d\nNumber Plate: %s\nType: %s\nCapacity: %.2f tons\n",
               v->vehicleID, v->numberPlate, v->vehicleType, v->capacity);
    }
}

/* GRAPH: LOCATIONS AND BFS ROUTING */

void displayLocations(void) {
    int i;
    printf("\nAvailable Locations:\n");
    for (i = 0; i < MAX_LOCATIONS; i++)
        printf("%d. %s\n", i + 1, locations[i]);
}

int findPath(int source, int destination, int path[]) {
    int queue[MAX_LOCATIONS], parent[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS] = {0};
    int front = 0, rear = 0, i, count = 0, current;

    for (i = 0; i < MAX_LOCATIONS; i++) parent[i] = -1;
    queue[rear++] = source;
    visited[source] = 1;

    while (front < rear) {
        current = queue[front++];
        for (i = 0; i < MAX_LOCATIONS; i++) {
            if (graph[current][i] && !visited[i]) {
                visited[i] = 1;
                parent[i] = current;
                queue[rear++] = i;
            }
        }
    }

    if (!visited[destination]) return 0;

    current = destination;
    while (current != -1) {
        path[count++] = current;
        current = parent[current];
    }
    return count;
}

void printPath(int path[], int count) {
    int i;
    for (i = count - 1; i >= 0; i--) {
        printf("%s", locations[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

void findRoute(void) {
    int source, destination, path[MAX_LOCATIONS], count;
    displayLocations();
    source = readInt("Enter Source Location (1-5): ") - 1;
    destination = readInt("Enter Destination Location (1-5): ") - 1;

    if (source < 0 || source >= MAX_LOCATIONS ||
        destination < 0 || destination >= MAX_LOCATIONS) {
        printf("Invalid location!\n");
        return;
    }
    if (source == destination) {
        printf("Source and destination are the same.\n");
        return;
    }

    count = findPath(source, destination, path);
    if (!count) printf("No route found!\n");
    else {
        printf("Route with the fewest connections:\n");
        printPath(path, count);
    }
}

/* LINKED LIST: TRIP MANAGEMENT */

void addTrip(void) {
    Trip *newTrip, *temp;
    Vehicle *vehicle;
    int path[MAX_LOCATIONS], count;

    newTrip = malloc(sizeof(Trip));
    if (!newTrip) {
        printf("Memory allocation failed!\n");
        return;
    }

    newTrip->tripID = readInt("Enter Trip ID: ");
    if (newTrip->tripID <= 0) {
        printf("Trip ID must be positive.\n");
        free(newTrip);
        return;
    }

    for (temp = tripHead; temp; temp = temp->next) {
        if (temp->tripID == newTrip->tripID) {
            printf("Trip ID already exists!\n");
            free(newTrip);
            return;
        }
    }

    newTrip->vehicleID = readInt("Enter Vehicle ID: ");
    vehicle = searchVehicle(root, newTrip->vehicleID);
    if (!vehicle) {
        printf("Vehicle does not exist! Add vehicle first.\n");
        free(newTrip);
        return;
    }

    displayLocations();
    newTrip->source = readInt("Select Source (1-5): ") - 1;
    newTrip->destination = readInt("Select Destination (1-5): ") - 1;

    if (newTrip->source < 0 || newTrip->source >= MAX_LOCATIONS ||
        newTrip->destination < 0 || newTrip->destination >= MAX_LOCATIONS ||
        newTrip->source == newTrip->destination) {
        printf("Invalid source or destination!\n");
        free(newTrip);
        return;
    }

    newTrip->load = readFloat("Enter Load in tons: ");
    if (newTrip->load <= 0) {
        printf("Load must be greater than zero.\n");
        free(newTrip);
        return;
    }
    if (newTrip->load > vehicle->capacity) {
        printf("Load exceeds vehicle capacity!\n");
        free(newTrip);
        return;
    }

    newTrip->next = NULL;
    if (!tripHead) tripHead = newTrip;
    else {
        temp = tripHead;
        while (temp->next) temp = temp->next;
        temp->next = newTrip;
    }

    printf("Trip added successfully!\n");
    count = findPath(newTrip->source, newTrip->destination, path);
    if (count) {
        printf("Suggested route: ");
        printPath(path, count);
    } else {
        printf("No route available.\n");
    }
}

void displayTrips(void) {
    Trip *temp = tripHead;
    if (!temp) {
        printf("No trip records available!\n");
        return;
    }

    printf("\n--- TRIP DETAILS ---\n");
    printf("%-10s %-12s %-22s %-22s %-10s\n",
           "Trip ID", "Vehicle ID", "Source", "Destination", "Load(t)");
    while (temp) {
        printf("%-10d %-12d %-22s %-22s %.2f\n",
               temp->tripID, temp->vehicleID,
               locations[temp->source], locations[temp->destination],
               temp->load);
        temp = temp->next;
    }
}

/* RELEASE DYNAMIC MEMORY */

void freeVehicles(Vehicle *node) {
    if (!node) return;
    freeVehicles(node->left);
    freeVehicles(node->right);
    free(node);
}

void freeTrips(void) {
    Trip *temp;
    while (tripHead) {
        temp = tripHead;
        tripHead = tripHead->next;
        free(temp);
    }
}

int main(void) {
    int choice;
    do {
        printf("\n========================================\n");
        printf(" GRANITE & HEAVY TRANSPORT MANAGEMENT\n");
        printf("========================================\n");
        printf("1. Add Vehicle\n");
        printf("2. Display All Vehicles\n");
        printf("3. Search Vehicle\n");
        printf("4. Add Transport Trip\n");
        printf("5. Display All Trips\n");
        printf("6. Find Route Using Graph\n");
        printf("7. Exit\n");
        printf("========================================\n");

        choice = readInt("Enter your choice: ");
        switch (choice) {
            case 1:
                addVehicle();
                break;
            case 2:
                if (!root) printf("No vehicles available!\n");
                else {
                    printf("\n--- VEHICLE DETAILS ---\n");
                    printf("%-10s %-18s %-20s %s\n",
                           "ID", "Number Plate", "Vehicle Type", "Capacity");
                    displayVehicles(root);
                }
                break;
            case 3:
                searchVehicleMenu();
                break;
            case 4:
                addTrip();
                break;
            case 5:
                displayTrips();
                break;
            case 6:
                findRoute();
                break;
            case 7:
                printf("Thank you for using the system!\n");
                break;
            default:
                printf("Invalid choice! Select 1 to 7.\n");
        }
    } while (choice != 7);

    freeVehicles(root);
    freeTrips();
    return 0;
}
