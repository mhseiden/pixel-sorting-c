#include "../include/parser.h"

#include <string>
#include <vector>
#include <iostream>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#define COPY_BUFFER_SIZE 4096

using namespace std;

// structural tokens
static const string SORT_TK	= string("SORT");
static const string BY_TK	= string("BY");
static const string WITH_TK	= string("WITH");
static const string RUNS_TK	= string("RUNS");
static const string THEN_TK	= string("THEN");

// orientation tokens
static const string ROW_TK	= string("ROWS");
static const string COL_TK	= string("COLS");

// sort direction tokens
static const string ASC_TK	= string("ASC");
static const string DESC_TK	= string("DESC");

// comparison tokens
static const string AVG_TK	= string("AVG");
static const string MUL_TK	= string("MUL");
static const string MAX_TK	= string("MAX");
static const string MIN_TK	= string("MIN");
static const string XOR_TK	= string("XOR");

// run type tokens
static const string FULL_TK	= string("FULL");
static const string DARK_TK	= string("DARK");
static const string LIGHT_TK	= string("LIGHT");
static const string FIXED_TK	= string("FIXED");

struct PixelSortSubquery;

typedef struct PixelSortQuery {
    size_t subquery_count;
    struct PixelSortSubquery * subqueries[256];
} PixelSortQuery_t;

typedef struct PixelSortSubquery {
    Orientation_e   orientation;
    Comparison_e    comparison;
    SortDirection_e sort_direction;
    RunType_e	    run_type;
    size_t	    run_type_param;
} PixelSortSubquery_t;

static int process_subquery(PixelSortSubquery_t *, const vector<string> &, size_t);
static void debug_subquery(const PixelSortSubquery_t *);

PixelSortQuery_t * process_tokens(const char* query_string) {

    // create a copy of the input and the token vector
    vector<string> tokens;
    char query_string_buffer[COPY_BUFFER_SIZE];
    char * token = strtok(strncpy(query_string_buffer, query_string, COPY_BUFFER_SIZE), " ");
    while(token) {
	if(0 < strlen(token)) {
	    tokens.push_back(string(token));
	    cerr << "Found token: " << token << endl;
	}
	token = strtok(NULL, " ");
    }


    // create and initialize the query instance
    PixelSortQuery_t * query = new PixelSortQuery_t;
    query->subquery_count = 0;


    // process tokens and create subqueries
    int token_idx = 0;
    do {
	const size_t subquery_idx = query->subquery_count++;
	PixelSortSubquery_t * subquery = new PixelSortSubquery_t;
	query->subqueries[subquery_idx] = subquery;
	token_idx = process_subquery(subquery, tokens, token_idx);
    } while(0 < token_idx);

    return query;
}

void destroy_query(PixelSortQuery_t * query) {
    for(int i = 0; i < query->subquery_count; ++i) {
	free(query->subqueries[i]);
    }
    free(query);
}

void debug_subquery(const PixelSortSubquery_t * subquery) {
    cerr << "Orientation: " << subquery->orientation << endl;
    cerr << "Comparison: " << subquery->comparison << endl;
    cerr << "Sort Direction: " << subquery->sort_direction << endl;
    cerr << "Run Type: " << subquery->run_type << endl;
    cerr << "Run Type Param: " << subquery->run_type_param << endl;
}

void debug_subquery(const struct PixelSortQuery * q, const int i) {
    const struct PixelSortSubquery * subquery = q->subqueries[i];
    cerr << "Debugging Subquery " << i << endl;
    debug_subquery(subquery);
}

int get_subquery_count(const struct PixelSortQuery * q) {
    return q->subquery_count; 
}

long get_run_threshold(const struct PixelSortQuery * q, const int i) {
    return q->subqueries[i]->run_type_param;
}

Orientation_e get_orientation(const struct PixelSortQuery * q, const int i) {
    return q->subqueries[i]->orientation;
}

RunType_e get_run_type(const struct PixelSortQuery * q, const int i) {
    return q->subqueries[i]->run_type;
}

Comparison_e get_comparison(const struct PixelSortQuery * q, const int i) {
    return q->subqueries[i]->comparison;
}

SortDirection_e get_sort_direction(const struct PixelSortQuery * q, const int i) {
    return q->subqueries[i]->sort_direction;
}

///////////////////////////////////
// static method definitions
///////////////////////////////////

int process_subquery(PixelSortSubquery_t * subquery, const vector<string> &tokens, size_t token_idx) {
    cerr << "Starting parse of subquery" << endl;


    // 1) ensure we're starting with a valid query
    const string sort_token = tokens.at(token_idx++);
    cerr << "Processing sort keyword token: " << sort_token << endl;
    if(0 != SORT_TK.compare(sort_token)) {
	cerr << "Expected first subquery token to be " << SORT_TK << endl;
	exit(1);
    }

    
    // 2) advance to the orientation token
    const string orientation_token = string(tokens[token_idx++]);
    cerr << "Processing orientation token: " << orientation_token << endl;

    if(0 == ROW_TK.compare(orientation_token)) {
	subquery->orientation = ROW;
    } else if (0 == COL_TK.compare(orientation_token)) {
	subquery->orientation = COLUMN;
    } else {
	cerr << "Sort orientation token is invalid: " << orientation_token << endl;
	exit(1);
    }
    

    // 3) advance to the sort order token
    const string sort_order_token = string(tokens[token_idx++]);
    cerr << "Processing sort order token: " << sort_order_token << endl;

    if(0 == ASC_TK.compare(sort_order_token)) {
	subquery->sort_direction = ASC;
    } else if (0 == DESC_TK.compare(sort_order_token)) {
	subquery->sort_direction = DESC;
    } else {
	cerr << "Sort order token is invalid: " << sort_order_token << endl;
	exit(1);
    }


    // 4) ensure we're at the "BY" token
    const string by_token = string(tokens[token_idx++]);
    cerr << "Processing by token: " << by_token << endl;
    if(0 != BY_TK.compare(by_token)) {
	cerr << "Expected comparator clause to begin with: " << BY_TK << endl;
	exit(1);
    }


    // 5) extract the comparator token
    const string comparator_token = string(tokens[token_idx++]);
    cerr << "Processing comparator token: " << comparator_token << endl;

    if(0 == AVG_TK.compare(comparator_token)) {
	subquery->comparison = AVG;
    } else if(0 == MUL_TK.compare(comparator_token)) {
	subquery->comparison = MUL;
    } else if(0 == MIN_TK.compare(comparator_token)) {
	subquery->comparison = MIN;
    } else if(0 == MAX_TK.compare(comparator_token)) {
	subquery->comparison = MAX;
    } else if(0 == XOR_TK.compare(comparator_token)) {
	subquery->comparison = XOR;
    } else {
	cerr << "Comparator token is invalid: " << comparator_token << endl;
	exit(1);
    }


    // 6) ensure we're at the "WITH" token
    const string with_token = string(tokens[token_idx++]);
    cerr << "Processing with token: " << with_token << endl;
    if(0 != WITH_TK.compare(with_token)) {
	cerr << "Expected run-type clause to begin with: " << WITH_TK << endl;
	exit(1);
    }
    

    // 7) extract the run type (and optional threshold)
    const string run_type_token = string(tokens[token_idx++]);
    cerr << "Processing run type token: " << run_type_token << endl;
    if(0 == FULL_TK.compare(run_type_token)) {
	subquery->run_type = FULL;
    } else if(0 == FIXED_TK.compare(run_type_token)) {
	subquery->run_type = FIXED;
	const string param_token = string(tokens[token_idx++]);
	cerr << "Processing param token: " << param_token << endl;
	subquery->run_type_param = stoi(param_token);
    } else if(0 == LIGHT_TK.compare(run_type_token)) {
	subquery->run_type = LIGHT;
	const string param_token = string(tokens[token_idx++]);
	cerr << "Processing param token: " << param_token << endl;
	subquery->run_type_param = stoi(param_token);
    } else if(0 == DARK_TK.compare(run_type_token)) {
	subquery->run_type = DARK;
	const string param_token = string(tokens[token_idx++]);
	cerr << "Processing param token: " << param_token << endl;
	subquery->run_type_param = stoi(param_token);
    } else {
	cerr << "Run Type token is invalid: " << run_type_token << endl;
	exit(1);
    }


    // 8) extract the final tokens and determine the return
    const string runs_token = string(tokens[token_idx++]);
    cerr << "Processing runs token: " << runs_token << endl;
    if(0 != RUNS_TK.compare(runs_token)) {
	cerr << "Expected run-type clause to end with: " << RUNS_TK << endl;
	exit(1);
    }


    // 9) see if there is another subquery, else we're at EOF
    if(tokens.size() > token_idx) {
	const string then_token = string(tokens[token_idx++]);
	cerr << "Processing then token: " << then_token << endl;
	if(0 != THEN_TK.compare(then_token)) {
	    cerr << "Expected another subquery but found: " << then_token << endl;
	    exit(1);
	} else {
	    cerr << "Next subquery begins at: " << token_idx << endl;
	    return token_idx;
	}
    } else {
	cerr << "End-Of-Input" << endl;
	return -1;
    }
}
