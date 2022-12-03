def count_long_subarray(A):
    '''
    Input:  A     | Python Tuple of positive integers
    Output: count | number of longest increasing subarrays of A
    '''
    count = 0
    ##################
    # YOUR CODE HERE #
    ##################
    if not A:
        return count
    cur_len = 1
    max_len = 0
    for i in range(1, len(A)):
        if A[i-1] < A[i]:
            cur_len += 1
        else:
            cur_len = 1
        if max_len < cur_len:
            count = 1
            max_len = cur_len
        elif max_len == cur_len:
            count += 1
    return count
