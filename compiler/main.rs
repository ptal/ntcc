use std::io;
mod parser;

fn parse_expr(line: &String) {
  match parser::expression(line.as_slice()) {
    Ok(()) => println!("Ok"),
    Err(err_msg) => println!("{}", err_msg)
  }
}

fn main() {
  for line in io::stdin().lines() {
    match line {
      Ok(ref s) => parse_expr(s),
      Err(_) => break
    }
  }
}