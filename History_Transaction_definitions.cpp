#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////


// Constructor
Transaction::Transaction( std::string ticker_symbol, unsigned int day_date, unsigned int month_date,  unsigned year_date, bool buy_sell_trans,  unsigned int number_shares, double trans_amount ){
  symbol = ticker_symbol;
  day = day_date;
  month = month_date;
  year = year_date;
  if(buy_sell_trans){
    trans_type="Buy";
  }
  else{
    trans_type="Sell";
  }
  shares =number_shares;
  amount = trans_amount;
  trans_id=assigned_trans_id;
  assigned_trans_id+=1;
  p_next=nullptr;
  acb=0.0;
  acb_per_share=0.0;
  share_balance=0;
  cgl=0;
}


// Destructor
Transaction::~Transaction(){}

// Overloaded < operator.
bool Transaction::operator<(Transaction const &other){
  if (year<other.year){
    return true;
  }
  if(month<other.month && year==other.year){
    return true;
  }
  if(day<other.day && year==other.year && month==other.month ){
    return true;
  }
  if(day==other.day && year==other.year && month==other.month){
    if(trans_id<other.trans_id){
      return true;
    }
  }
  return false;
}

// GIVEN
// Member functions to get values.
//

std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
History::History(){
  p_head=nullptr;
}

// Destructor
History::~History(){
  Transaction *current = p_head;
  Transaction *next=nullptr;
  while (current!= nullptr) {
      next = current->get_next();
      delete current;
      current=next;
  }
}

// insert(...): Insert transaction into linked list.
void History::insert( Transaction *p_new_trans ){
  if (p_head == nullptr) {
    p_head = p_new_trans;
  } 
  else {
    Transaction *current = p_head;
    while (current->get_next() != nullptr) {
        current = current->get_next();
    }
    current->set_next(p_new_trans); 
  }
}

// read_history(...): Read the transaction history from file.
void History::read_history(){
    ece150::open_file();

    while (ece150::next_trans_entry()) {
      Transaction *newtrans = new Transaction(ece150::get_trans_symbol(), ece150::get_trans_day(), 
      ece150::get_trans_month(),ece150::get_trans_year(), ece150::get_trans_type(),
      ece150::get_trans_shares(), ece150::get_trans_amount());
      this->insert(newtrans);  
    }

    ece150::close_file();
}

// print() Print the transaction history.
void History::print(){
  std::cout<<"========== BEGIN TRANSACTION HISTORY ==========";
 
  Transaction *current = p_head;
  
  while (current!=nullptr) {
    current->print();
    current=current->get_next();
  }

  std::cout<<"========== END TRANSACTION HISTORY ==========";

}

// sort_by_date(): Sort the linked list by trade date.
void History::sort_by_date(){
  Transaction *previous= p_head;
  Transaction *current = previous->get_next();

  while(current!=nullptr){
    //if the one before is bigger
    if(current<previous){
      while(current<current->get_next()){
        //detach
        previous->set_next(current->get_next());
        //move the current node to begining
        current->set_next(p_head);
        p_head=current;
        //make next item the bigger one
        current=previous;    
      } 
      previous=p_head;
    }

    //if it's normal 
    else{
      previous=current;
      current=current->get_next();
    }

  }  
}

// update_acb_cgl(): Updates the ACB and CGL values.
void History::update_acb_cgl(){
  Transaction *current=p_head;

  //set abc cgl for ver first trans
   if (current->get_trans_type()==true){
      //buy
      current->set_acb((current->get_amount()));
      current->set_share_balance((current->get_shares()));
      current->set_acb_per_share((current->get_acb()/current->get_share_balance()));
    }

    else{
      //sell
      current->set_acb(-(current->get_amount()));
      current->set_share_balance(-(current->get_shares()));
      current->set_acb_per_share((current->get_acb()/current->get_share_balance()));
      current->set_cgl((current->get_amount()-current->get_shares()));
    }
 
  //set abc cgl for rest
  while(current->get_next()!=nullptr){
    Transaction *next=current->get_next(); 

    if (next->get_trans_type()==true){
      //buy
      next->set_acb((current->get_acb()+next->get_amount()));
      next->set_share_balance((current->get_share_balance()+next->get_shares()));
      next->set_acb_per_share((next->get_acb()/next->get_share_balance()));
    }

    else{
      //sell
      next->set_acb((current->get_acb()-(next->get_shares()*current->get_acb_per_share())));
      next->set_share_balance((current->get_share_balance()-next->get_shares()));
      next->set_acb_per_share((next->get_acb()/next->get_share_balance()));
      next->set_cgl(next->get_amount()-(next->get_shares()*current->get_acb_per_share()));
    }
  
    current=next;
  }
}

// compute_cgl(): )Compute the ACB, and CGL.
double History::compute_cgl( unsigned int year ){
  update_acb_cgl();
  double totalcgl=0;

  Transaction *current=p_head;
  while(current->get_next()!=nullptr){
    Transaction *next=current->get_next();

    if(current->get_year()==year){
      totalcgl+=current->get_cgl();
    }

    current=next;
  }
  
  return totalcgl;
}

// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }


//g++ -std=c++11 History_Transaction_definitions.cpp project4.cpp main.cpp -o main