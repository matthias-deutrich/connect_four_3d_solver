fn n_choose_k(n: u128, k: u128) -> u128 {
    let mut result: u128 = 1;
    let mut i: u128 = 0;
    while i < k {
        result *= n - i;
        i += 1;
        result /= i;
    }
    result
}

fn distribute_n_pieces(mut n: u128) -> u128 {
    let mut result: u128 = n_choose_k(16 + n - 1, n);
    let mut full_cols = 1;
    while n > 4 {
        n -= 5;
        //result -= n_choose_k(16, full_cols) * distribute_n_pieces(n);
        result -= n_choose_k(16 + full_cols - 1, full_cols) * distribute_n_pieces(n);
        full_cols += 1;
    }
    result
}

fn n_piece_board_count(n: u128) -> u128 {
    distribute_n_pieces(n) * n_choose_k(n, n / 2)
}

fn main() {
    //let mut last : u128 = 0;
    for x in 1..65 {
        let curr : u128 = n_piece_board_count(x);
        //if curr < last {
        //    println!("{} is smaller than previous iteration.", x);
        //}
        //else {
        //    println!("All OK.");
        //}
        //println!("{}:   {}", x, curr);
        println!("{} & {} \\\\", x, curr);
        //last = curr;
    }
    //println!("{}", distribute_n_pieces(62));
    //println!("{}", n_piece_board_count(20));
}
