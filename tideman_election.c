#include <cs50.h>
#include <stdio.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

typedef struct
{
    int strength;
    pair a_pair;
} pair_strength;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];
int strengths[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool compare_strings(string one, string two);
void merge_sort(pair_strength nums[], int nums_length);
void merge_halfs(pair_strength nums[], pair_strength la[], int la_len, pair_strength ra[], int ra_len);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        //     for (int i = 0; i < candidate_count; i++) {
        //         for (int j = 0; j < candidate_count; j++) {
        //             printf("%i ", preferences[i][j]);
        //             if (j == candidate_count - 1) {
        //                 printf("\n");
        //             }
        //         }
        //     }

        //        printf("Above is the preferences matrix\n");

        //      printf("\n");
    }

    add_pairs();

    // for (int i = 0; i < candidate_count; i++)
    // {
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         printf("%i ", preferences[i][j]);
    //         if (j == candidate_count - 1)
    //         {
    //             printf("\n");

    //         }
    //     }
    // }

    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf("These are the pairs: {Winner: %i, Loser: %i}\n", pairs[i].winner, pairs[i].loser);
    // }

    // printf("The pair count is: %i\n", pair_count);

    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf("These are the strengths: %i\n", strengths[i]);
    // }

    sort_pairs();


    //   for (int i = 0; i < pair_count; i++)
    // {
    //     printf("Pairs after sort: {Winner: %i, Loser: %i}\n", pairs[i].winner, pairs[i].loser);
    // }


    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (compare_strings(name, candidates[i]))
        {
            ranks[rank] = i;
            return true;
        }
    }
    // TODO
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int voted_candidates[candidate_count];
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    // TODO
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count-1; i++)
    {
        for (int j = i+1; j < candidate_count; j++)
        {
            int ij = preferences[i][j];
            int ji = preferences[j][i];
            pair a_pair;

            if (ij == ji)
                continue;
            if (ij > ji)
            {
                a_pair.winner = i;
                a_pair.loser = j;
                strengths[pair_count] = ij;
            }
            else
            {
                a_pair.winner = j;
                a_pair.loser = i;
                strengths[pair_count] = ji;

            }
            pairs[pair_count] = a_pair;
            pair_count++;
        }
    }
    // TODO
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    pair_strength pair_strengths[pair_count];

    for (int i = 0; i < pair_count; i++)
    {
        pair_strength a_pair_strength;
        a_pair_strength.strength = strengths[i];
        a_pair_strength.a_pair = pairs[i];
        pair_strengths[i] = a_pair_strength;
    }

    for (int i = 0; i < pair_count; i++)
    {
        int strength_i = pair_strengths[i].strength;
        int winner_i = pair_strengths[i].a_pair.winner;
        for (int j = 0; j < pair_count; j++)
        {
            if (i == j)
                continue;
            int strength_j = pair_strengths[j].strength;
            int winner_j = pair_strengths[j].a_pair.winner;
            if (winner_i == winner_j)
            {
                pair_strengths[i].strength +=  strengths[j];
                // pair_strengths[j].strength += strength_i;
            }
        }
    }

    // printf("Before sort\n");

    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf("strength: %i, (winner: %i, loser: %i)\n", pair_strengths[i].strength, pair_strengths[i].a_pair.winner, pair_strengths[i].a_pair.loser);
    // }

    merge_sort(pair_strengths, pair_count);


    for (int i = 0; i < pair_count; i++)
    {
        pairs[i] = pair_strengths[pair_count - i - 1].a_pair;
    }


    // printf("after sort\n");

    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf("strength: %i, (winner: %i, loser: %i)\n", pair_strengths[i].strength, pair_strengths[i].a_pair.winner, pair_strengths[i].a_pair.loser);
    // }

    // printf("These are the sorted pairs\n");

    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf("(winner: %i, loser: %i)\n", pairs[i].winner, pairs[i].loser);
    // }


    // TODO
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int cycles[pair_count];
    int cycle_counter = 0;
    int is_cycle;
    for (int i = 0; i < pair_count; i++)
    {
        is_cycle = 0;
        int row = pairs[i].winner;
        int col = pairs[i].loser;
        for (int j = 0; j < cycle_counter; j++)
        {
            if (col == cycles[j])
            {
                is_cycle = 1;
            }
        }
        if (is_cycle == 1)
        {
            continue;
        }
        else
        {
            locked[row][col] = true;
            cycles[i] = row;
            cycle_counter++;
        }
    }

    // for (int i = 0; i < candidate_count; i++)
    // {
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         printf("%i ", locked[i][j]);
    //         if (j == candidate_count - 1)
    //         {
    //             printf("\n");
    //         }
    //     }
    // }

    // TODO
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int row_max = 0;
    int winning_row;
    for (int i = 0; i < candidate_count; i++)
    {
        int col_max = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j] > 0)
                col_max += 1;
        }
        if (col_max > row_max)
        {
            row_max = col_max;
            winning_row = i;
        }
    }
    printf("%s\n", candidates[winning_row]);
    // TODO
    return;
}

bool compare_strings(string one, string two)
{
    for (int i = 0; one[i] != '\0'; i++)
    {
        if (one[i] != two[i])
            return false;
    }
    for (int i = 0; two[i] != '\0'; i++)
    {
        if (two[i] != one[i])
            return false;
    }
    return true;
}

void merge_sort(pair_strength nums[], int nums_length)
{
    if (nums_length == 1)
        return;
    int mid = nums_length / 2;
    pair_strength la[mid];
    pair_strength ra[nums_length - mid];
    for (int i = 0; i < mid; i++)
    {
        la[i] = nums[i];
    }
    for (int i = 0; i < nums_length - mid; i++)
    {
        ra[i] = nums[mid + i];
    }
    merge_sort(la, mid);
    merge_sort(ra, nums_length - mid);
    merge_halfs(nums, la, mid, ra, nums_length - mid);
}

void merge_halfs(pair_strength nums[], pair_strength la[], int la_len, pair_strength ra[], int ra_len)
{
    pair_strength merged_array[la_len + ra_len];
    int la_i = 0;
    int ra_i = 0; // these are the left, right, and merged arrays respectively
    int ma_i = 0;

    while (la_i < la_len && ra_i < ra_len)
    {
        if (la[la_i].strength < ra[ra_i].strength)
        {
            merged_array[ma_i] = la[la_i];
            la_i++;
        }
        else
        {
            merged_array[ma_i] = ra[ra_i];
            ra_i++;
        }
        ma_i++;
    }
    while (la_i < la_len)
    {
        merged_array[ma_i] = la[la_i];
        la_i++;
        ma_i++;
    }
    while (ra_i < ra_len)
    {
        merged_array[ma_i] = ra[ra_i];
        ra_i++;
        ma_i++;
    }
    for (int i = 0; i < (la_len + ra_len); i++)
    {
        nums[i] = merged_array[i];
        //printf("Merged array element %i: %i\n", i+1,merged_array[i]);
    }
}
