#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Fixed “data” per the problem statement */
#define SEATS   24      /* number of seats on the plane */
#define NAMELEN 20      /* max chars stored for a name, +1 for '\0' */

/* A single seat’s data. Keeping it tiny and beginner-friendly. */
typedef struct {
    int  id;                 /* seat number: 1..24 */
    int  assigned;           /* 0 = empty, 1 = taken */
    char first[NAMELEN];     /* passenger first name (no spaces) */
    char last[NAMELEN];      /* passenger last name  (no spaces) */
} Seat;

/* One flight = array of 24 Seat records. */
typedef struct {
    Seat seat[SEATS];
} Flight;

/* ---------- Function Prototypes (short summaries) ---------- */
void init_flight(Flight *f);                          /* set all seats empty */
void second_menu(Flight *f, const char *title);       /* per-flight actions */
void show_num_empty(const Flight *f);                 /* print count empty */
void show_empty_list(const Flight *f);                /* print numbers empty */
void show_alpha_list(const Flight *f);                /* print assigned A→Z */
void assign_seat(Flight *f);                          /* prompt + assign */
void delete_seat(Flight *f);                          /* prompt + delete */
int  count_empty(const Flight *f);                    /* helper counter */
void clear_line(void);                                /* flush stdin line */
int  name_cmp(const Seat *a, const Seat *b);          /* last, then first */

/* -------------------- Implementation -------------------- */

/* init_flight: give every seat an ID and mark as empty. */
void init_flight(Flight *f) {
    int i;
    for (i = 0; i < SEATS; i++) {
        f->seat[i].id = i + 1;         /* seats are 1..24 */
        f->seat[i].assigned = 0;       /* start with no reservations */
        f->seat[i].first[0] = '\0';
        f->seat[i].last[0]  = '\0';
    }
}

/* clear_line: discard leftover characters until newline.
   This keeps our next scanf() clean and is common in beginner C. */
void clear_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { /* discard */ }
}

/* count_empty: simple tally of seats not assigned. */
int count_empty(const Flight *f) {
    int i, c = 0;
    for (i = 0; i < SEATS; i++)
        if (!f->seat[i].assigned) c++;
    return c;
}

/* show_num_empty: print “N out of 24” to the screen. */
void show_num_empty(const Flight *f) {
    printf("Empty seats: %d out of %d\n\n", count_empty(f), SEATS);
}

/* show_empty_list: print all seat numbers that are currently empty. */
void show_empty_list(const Flight *f) {
    int i, any = 0;
    printf("Empty seat numbers: ");
    for (i = 0; i < SEATS; i++) {
        if (!f->seat[i].assigned) {
            printf("%d ", f->seat[i].id);
            any = 1;
        }
    }
    if (!any) printf("(none)");
    printf("\n\n");
}

/* name_cmp: compares the two assigned seats by last name, then first name.
   Case-sensitive on purpose (keeps it beginner-simple). */
int name_cmp(const Seat *a, const Seat *b) {
    int c = strcmp(a->last, b->last);
    if (c == 0) c = strcmp(a->first, b->first);
    return c;
}

/* show_alpha_list: builds a list of indices for assigned seats,
   selection-sort those indices alphabetically, then print.
   We sort indices (not the actual seats) so seat IDs don’t move. */
void show_alpha_list(const Flight *f) {
    int idx[SEATS];
    int i, j, n = 0;

    /* collects which seats are assigned */
    for (i = 0; i < SEATS; i++)
        if (f->seat[i].assigned) idx[n++] = i;

    printf("Alphabetical list of assigned seats:\n");
    if (n == 0) { puts("(none)\n"); return; }

    /* selection sort on indices (A→Z by last, then first) */
    for (i = 0; i < n - 1; i++) {
        int minpos = i;
        for (j = i + 1; j < n; j++) {
            if (name_cmp(&f->seat[idx[j]], &f->seat[idx[minpos]]) < 0)
                minpos = j;
        }
        if (minpos != i) {
            int tmp = idx[i];
            idx[i] = idx[minpos];
            idx[minpos] = tmp;
        }
    }

    /* print sorted passengers */
    for (i = 0; i < n; i++) {
        int k = idx[i];
        printf("Seat %2d: %s %s\n",
               f->seat[k].id, f->seat[k].first, f->seat[k].last);
    }
    printf("\n");
}

/* assign_seat: ask for a seat number and a name; 0 cancels.
   Validates range and whether the seat is already taken. */
void assign_seat(Flight *f) {
    int seatno;
    char first[NAMELEN], last[NAMELEN];

    printf("\nAssign a customer to a seat (enter 0 to cancel)\n");
    show_empty_list(f);
    if (count_empty(f) == 0) { puts("No empty seats.\n"); return; }

    /* seat number prompt */
    printf("Seat number (1-%d, 0=cancel): ", SEATS);
    if (scanf("%d", &seatno) != 1) { clear_line(); puts("Invalid input.\n"); return; }
    clear_line();
    if (seatno == 0) { puts("Assignment canceled.\n"); return; }
    if (seatno < 1 || seatno > SEATS) { puts("Seat out of range.\n"); return; }

    if (f->seat[seatno - 1].assigned) {
        printf("Seat %d is already taken.\n\n", seatno);
        return;
    }

    /* read simple one-word names (fits the assignment level) */
    printf("First name (no spaces): ");
    if (scanf("%19s", first) != 1) { clear_line(); puts("Invalid input.\n"); return; }
    clear_line();

    printf("Last name (no spaces): ");
    if (scanf("%19s", last) != 1) { clear_line(); puts("Invalid input.\n"); return; }
    clear_line();

    /* store data into the chosen seat */
    strcpy(f->seat[seatno - 1].first, first);
    strcpy(f->seat[seatno - 1].last,  last);
    f->seat[seatno - 1].assigned = 1;

    printf("Assigned seat %d to %s %s.\n\n", seatno, first, last);
}

/* delete_seat: ask for seat number, confirm, then clear.
   0 cancels quickly so the user can back out. */
void delete_seat(Flight *f) {
    int seatno;
    char ans;

    printf("\nDelete a seat assignment (enter 0 to cancel)\n");
    printf("Seat number to clear (1-%d, 0=cancel): ", SEATS);
    if (scanf("%d", &seatno) != 1) { clear_line(); puts("Invalid input.\n"); return; }
    clear_line();

    if (seatno == 0) { puts("Delete canceled.\n"); return; }
    if (seatno < 1 || seatno > SEATS) { puts("Seat out of range.\n"); return; }

    if (!f->seat[seatno - 1].assigned) {
        printf("Seat %d is already empty.\n\n", seatno);
        return;
    }

    printf("Confirm delete for seat %d (%s %s)? (y/n): ",
           seatno,
           f->seat[seatno - 1].first,
           f->seat[seatno - 1].last);

    if (scanf(" %c", &ans) != 1) { clear_line(); puts("Invalid input.\n"); return; }
    clear_line();
    ans = (char)tolower((unsigned char)ans);

    if (ans == 'y') {
        f->seat[seatno - 1].assigned = 0;
        f->seat[seatno - 1].first[0] = '\0';
        f->seat[seatno - 1].last[0]  = '\0';
        puts("Seat cleared.\n");
    } else {
        puts("No changes made.\n");
    }
}

/* second_menu: loop until the user chooses to return.
   Each choice affects only the passed-in Flight. */
void second_menu(Flight *f, const char *title) {
    char choice;

    for (;;) {
        printf("\nSecond Level Menu – %s\n", title);
        puts("a) Show number of empty seats");
        puts("b) Show list of empty seats");
        puts("c) Show alphabetical list of seats");
        puts("d) Assign a customer to a seat assignment");
        puts("e) Delete a seat assignment");
        puts("f) Return to Main menu");
        printf("Enter choice: ");

        if (scanf(" %c", &choice) != 1) { clear_line(); puts("Invalid input.\n"); continue; }
        clear_line();
        choice = (char)tolower((unsigned char)choice);

        switch (choice) {
            case 'a': show_num_empty(f);    break;
            case 'b': show_empty_list(f);   break;
            case 'c': show_alpha_list(f);   break;
            case 'd': assign_seat(f);       break;
            case 'e': delete_seat(f);       break;
            case 'f': return;               /* go back to first menu */
            default : puts("Invalid choice.\n"); break;
        }
    }
}

/* main: create the two flights and drive the menus. */
int main(void) {
    Flight outbound, inbound;
    char choice;

    init_flight(&outbound);
    init_flight(&inbound);

    puts("Welcome to Colossus Airlines Seat Reservation");

    for (;;) {
        puts("\nFirst Level Menu");
        puts("a) Outbound Flight");
        puts("b) Inbound Flight");
        puts("c) Quit");
        printf("Enter choice: ");

        if (scanf(" %c", &choice) != 1) { clear_line(); puts("Invalid input.\n"); continue; }
        clear_line();
        choice = (char)tolower((unsigned char)choice);

        if (choice == 'a')
            second_menu(&outbound, "Outbound");
        else if (choice == 'b')
            second_menu(&inbound, "Inbound");
        else if (choice == 'c') {
            puts("Goodbye!");
            break;
        } else {
            puts("Invalid choice.");
        }
    }
    return 0;
}