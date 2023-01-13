def merge(left, right):
    n_left, n_right = len(left), len(right)
    i_left, i_right = 0, 0
    B = []
    x = 0
    while i_left + i_right < n_left + n_right:
        if i_left < n_left:
            k1, s1, t1 = left[i_left]
        if i_right < n_right:
            k2, s2, t2 = right[i_right]
        if i_left == n_left:
            k, s, x = k2, max(x, s2), t2
            i_right += 1
        elif i_right == n_right:
            k, s, x= k1, max(x, s1), t1
            i_left += 1
        else:
            if x < min(s1, s2):
                x = min(s1, s2)
            if t1 <= s2:
                k, s, x = k1, x, t1
                i_left += 1
            elif t2 <= s1:
                k, s, x = k2, x, t2
                i_right += 1
            elif x < s2:
                k, s, x = k1, x, s2
            elif x < s1:
                k, s, x = k2, x, s1
            else:
                k, s, x = k1 + k2, x, min(t1, t2)
                if t1 == x: i_left += 1
                if t2 == x: i_right += 1
        B.append((k, s, x))
    new_b = [B[0]]
    for k, s, t in B[1:]:
        k_, s_, t_ = new_b[-1]
        if k == k_ and t_ == s:
            new_b.pop()
            s = s_
        new_b.append((k, s, t))
    return new_b

        

def satisfying_booking(R):
    '''
    Input:  R | Tuple of |R| talk request tuples (s, t)
    Output: B | Tuple of room booking triples (k, s, t)
              | that is the booking schedule that satisfies R
    '''
    n = len(R)
    if n == 1:
        s, t = R[0]
        return ((1, s, t),)
    m = n // 2
    left, right = R[:m], R[m:]
    left_b = satisfying_booking(left)
    right_b = satisfying_booking(right)
    B = merge(left_b, right_b)
    ##################
    # YOUR CODE HERE #
    ##################
    return tuple(B)
