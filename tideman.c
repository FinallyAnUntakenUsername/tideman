#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
}
pair;

typedef char *string;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool test(int end, int start);

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

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    string holder[candidate_count];
    
    for(int i = 0; i < candidate_count; i++)
    {	//If the name matches with the candidates, then it's going to check if the voter already has registered the name.
        if(strcmp(name, candidates[i]) == 0)
        {   
            holder[rank] = name;
            
            for(int p = 0; p < rank; p++)
            {
                if(strcmp(name, holder[p]) == 0)
                {
                    return false;
                }
            }
            //It puts the candidate's number in the array after ranking.
            ranks[rank] = i;
                
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for(int i = 0; i < candidate_count; i++)
    {
        for(int j = i + 1; j < candidate_count; j++)
        {    
	     //Counts the amount of people that prefer candidate i over j.
             preferences[ranks[i]][ranks[j]] += 1;  
        }
    }
    return;
}
// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for(int i = 0; i < candidate_count; i++)
    {
        for(int j = i + 1; j < candidate_count; j++)
        {
            
            if(preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if(preferences[j][i] > preferences[i][j])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    
    int holder[pair_count];

    for(int p = 0; p < pair_count; p++)
    {
        holder[p] = preferences[pairs[p].winner][pairs[p].loser] - preferences[pairs[p].loser][pairs[p].winner];    
    }
    
    //Are going to be used to sort the candidates.
    int rhand;
    int lhand;
    int erdhand;
    
    for(int i = 0; i < pair_count - 1; i++)
    {	
	//Just a bubble sort where it sorts pairs after how much the winner wins over the loser. The pairs with the biggest difference goes first.
        if(holder[i] < holder[i + 1])
        {
	    //Sorts value of the wins.
            rhand = holder[i + 1];
            
            holder[i + 1] = holder[i];
            
            holder[i] = rhand;
            
            //Sorts the pairs.
            lhand = pairs[i + 1].winner;
            
            pairs[i + 1].winner = pairs[i].winner;
            
            pairs[i].winner = lhand;
             
            erdhand = pairs[i + 1].loser;
            
            pairs[i + 1].loser = pairs[i].loser;
            
            pairs[i].loser = erdhand;
        }  
    }
    return;
}

//A recursion function, that look for if there a cycle(going to be used in the next function).
bool test(int end, int start)
{   
    //Looks for whether you can get back to the winner of the pair through the loser, alas a cycle(recursive basecase).
    if (end == start)
    {
        return true;
    }   
    
    //Looks through the candidates.
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[end][i])
        {
            if (test(i, start))
            {
                return true;
            }
        }
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    //Locks a pair (locks means showing an arrow from the winner to the loser) unless it's going to create a loop.
    for (int i = 0; i < pair_count; i++)
    {
        if (!test(pairs[i].loser, pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int p;
  
    //Prints out the winner, which is the candidate that didn't have any arrow at them (them haven't lost head to head against another candidate).
    for (int i = 0; i < candidate_count; i++)
    {
        p = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == false)
            {
                p++;
            }
        }


        if (p == candidate_count)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

