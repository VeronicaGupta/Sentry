#include "recognition.h"
//--------------------------FUNCTIONS AND THRESHOLD--------------------------------------
//this function finds the minimum between three values since no min function in C
//parameters are the three values from the matrix
uint32_t find_min(uint32_t v1, uint32_t v2, uint32_t v3){
    uint32_t minimum = (v1 < v2) ? ((v1 < v3) ? v1 : v3) : ((v2 < v3) ? v2 : v3); 
    return minimum;
}
//this function finds the dst cost between two data sets
//the higher the cost, the more apart the two data sets are
//parameters are the two data sets. 
//this function does not utuilize sakoe and chiba dtw shortcut, it is the original calculation for dtw
//threshold for this project is 400,000 for medium to slow gestures
uint32_t dtw_cost(int32_t *set_one, int32_t *set_two){
    uint32_t dtw_mat[SET_SIZE][SET_SIZE] = {0}; //initialize matrix
    //----------------calculation of matrix------------------------
    uint32_t cost = 0;
    int i;
    //calculate first lowest, leftest element of dtw----------------------
    if((set_one[0] > 0) && (set_two[0] <0)){
            dtw_mat[0][0] = abs(abs(set_one[0]) + abs(set_two[0]));
        }
        
    else if ((set_one[0] < 0) && (set_two[0] >0))
        {
            dtw_mat[0][0] = abs(abs(set_one[0]) + abs(set_two[0]));
        }
        
    else{
            dtw_mat[0][0] = abs(abs(set_one[0]) - abs(set_two[0]));
        }
    //calculate first column now--------------------------
    for(i = 1; i < SET_SIZE; i++){
        if((set_one[i] > 0) && (set_two[0] <0))
            dtw_mat[i][0] = abs(abs(set_one[i]) + abs(set_two[0])) + dtw_mat[i-1][0];
        
        else if ((set_one[0] < 0) && (set_two[0] >0))
            dtw_mat[i][0] = abs(abs(set_one[i]) + abs(set_two[0])) + dtw_mat[i-1][0];

        else
            dtw_mat[i][0] = abs(abs(set_one[i]) - abs(set_two[0])) + dtw_mat[i-1][0];
        
    }

    //now calculate the first row
    for(i = 1; i < SET_SIZE; i++){
        if((set_one[0] > 0) && (set_two[i] <0))
            dtw_mat[0][i] = abs(abs(set_one[0]) + abs(set_two[i])) + dtw_mat[0][i-1];
        
        else if ((set_one[0] < 0) && (set_two[0] >0))
            dtw_mat[0][i] = abs(abs(set_one[0]) + abs(set_two[i])) + dtw_mat[0][i-1];

        else
            dtw_mat[0][i] = abs(abs(set_one[0]) - abs(set_two[i])) + dtw_mat[0][i-1];
        
    }

    //now since first row and column are finished. Calculate the rest of the matrix
    for(i = 1; i < SET_SIZE; i++){
        for(int j = 1; j < SET_SIZE; j++){
            if((set_one[i] > 0) && (set_two[j] <0))
            dtw_mat[i][j] = abs(abs(set_one[i]) + abs(set_two[j])) + find_min(dtw_mat[i-1][j-1], dtw_mat[i-1][j], dtw_mat[i][j-1]);
        
        else if ((set_one[i] < 0) && (set_two[j] >0))
            dtw_mat[i][j] = abs(abs(set_one[i]) + abs(set_two[j])) + find_min(dtw_mat[i-1][j-1], dtw_mat[i-1][j], dtw_mat[i][j-1]);

        else
            dtw_mat[i][j] = abs(abs(set_one[i]) - abs(set_two[j])) + find_min(dtw_mat[i-1][j-1], dtw_mat[i-1][j], dtw_mat[i][j-1]);
        }
    }
    //matrix has been printed for testing!
    for(i = SET_SIZE-1; i >=0; i--){
        for(int j = 0; j < SET_SIZE; j++){
            // printf("%lu ", dtw_mat[i][j]);
        }
        printf("\n");
    }
    
    //--------------------------create the cost path and check!----------------------------------
    cost += dtw_mat[SET_SIZE -1][SET_SIZE -1]; // add the last spot of the array to get started,
    int col = SET_SIZE -1;
    int row = SET_SIZE -1;
    uint32_t val = 0; //value of the minimum
    for(i = SET_SIZE-2; i >= 0; i--){ //its minus 2 because the last value is already added, the extra is for the array limit
        val = find_min(dtw_mat[row-1][col-1], dtw_mat[row-1][col], dtw_mat[row][col-1]);
        //if you picked dtw_mat[row-1][col-1] then both the row and the column must be subtracted by one
        if (val == dtw_mat[row-1][col-1]){
            row--;
            col--;
        }
        //if just the row is subtracted then adjust the row only
        else if (dtw_mat[row-1][col])
            row--;
        //if the minimum is at dtw_mat[row][col-1], subtract only the column
        else
            col--;
        //after adjusting the row and column indices, add the value to the total cost.
        cost += val;
    }
    //return that final cost
    return cost;

}
//------------------------------END OF FUNCTIONS-----------------------------------------------------