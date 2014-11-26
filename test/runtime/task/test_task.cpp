#include "task.h"
#include "stdio.h"
#include "assert.h"
#include "test.h"

test_results_t test_task_allocate(){
	test_results_t r = new_results(); 
	
	print_test("allocation");
	Task * t = new Task();
	print_assert("id is 0", tassert(&r, (t->getId() == 0)));
	print_assert("rank is 0", tassert(&r, (t->getRank() == 0)));
	print_assert("size is 0", tassert(&r, (t->getMaxSize() == 0)));
	print_test("update task size 5");
	t->update(1,2,5);
	print_assert("id is 1", tassert(&r, (t->getId() == 1)));
	print_assert("rank is 2", tassert(&r, (t->getRank() == 2)));
	print_assert("size is 5", tassert(&r, (t->getMaxSize() == 5)));
	print_test("update task size 15");
	t->update(4,8,15);
	print_assert("id is 4", tassert(&r, (t->getId() == 4)));
	print_assert("rank is 8", tassert(&r, (t->getRank() == 8)));
	print_assert("size is 15", tassert(&r, (t->getMaxSize() == 15)));
	print_test("update task size 10");
	t->update(6,12,10);
	print_assert("id is 6", tassert(&r, (t->getId() == 6)));
	print_assert("rank is 12", tassert(&r, (t->getRank() == 12)));
	print_assert("size is 15", tassert(&r, (t->getMaxSize() == 15)));
	//print_results("", r);
	delete t;
	return r;
}
test_results_t test_task_pack_unpack(){
	test_results_t r = new_results(); 
	
	Task * t = new Task();
	//begin pack
	print_test("test empty pack");
	t->update(1,2,100);
	t->startPack();
	print_assert("effective size is sizeof(int)*2", tassert(&r, (t->getEffectiveSize() == sizeof(int)*2)));
	t->update(4,5,100);
	t->startUnpack();
	print_assert("id is 1", tassert(&r, (t->getId() == 1)));
	print_assert("rank is 2", tassert(&r, (t->getRank() == 2)));
	
	print_test("test pack");
	int q = 5431; int q2 = 1234; int qres= -1;
	t->update(1,2,100);
	t->startPack();
	t->pack(q);
	t->pack(q2);
	t->update(4,5,100);
	t->startUnpack();
	print_assert("id is 1", tassert(&r, (t->getId() == 1)));
	print_assert("rank is 2", tassert(&r, (t->getRank() == 2)));
	t->unpack(&qres);
	print_assert("data point 1 is 5431", tassert(&r, (qres == q)));
	t->unpack(&qres);
	print_assert("data point 2 is 1234", tassert(&r, (qres == q2)));
	
	print_test("test array pack");
	int p[] = {1,2,3,4,5};
	int* p2;
	t->update(10,20,100);
	t->startPack();
	t->pack((int *) p, 5);
	t->pack(q);
	t->pack((int *) p, 3);
	t->update(4,5,100);
	t->startUnpack();
	print_assert("id is 10", tassert(&r, (t->getId() == 10)));
	print_assert("rank is 20", tassert(&r, (t->getRank() == 20)));
	t->unpack(&p2);
	print_assert("data point 1 in array p is 1", tassert(&r, (p2[0] == p[0])));
	print_assert("data point 2 in array p is 2", tassert(&r, (p2[1] == p[1])));
	print_assert("data point 3 in array p is 3", tassert(&r, (p2[2] == p[2])));
	print_assert("data point 4 in array p is 4", tassert(&r, (p2[3] == p[3])));
	print_assert("data point 5 in array p is 5", tassert(&r, (p2[4] == p[4])));
	t->unpack(&qres);
	print_assert("data point 2 in scalar is 5431", tassert(&r, (qres == q)));
	p2[0] = p2[1] = p2[2] = -1;
	t->unpack(&p2);
	print_assert("data point 1 in array p is 1", tassert(&r, (p2[0] == p[0])));
	print_assert("data point 2 in array p is 2", tassert(&r, (p2[1] == p[1])));
	print_assert("data point 3 in array p is 3", tassert(&r, (p2[2] == p[2])));
	//print_results("", r);
	delete t;
	return r;
}
test_results_t test_task(){
		test_results_t child, parent;
		parent = new_results();
		
		print_category("task");
		child = test_task_allocate();
		parent = combine_results(parent, child);
		child = test_task_pack_unpack();
		parent = combine_results(parent, child);
		
		//print_results("", parent);
		return parent;
}
