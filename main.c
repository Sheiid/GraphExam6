/*
 * main.c
 *!!!!NOT WORKING !!!!
 *
 * weighted directed graph stored in a file gives 6 parameters from the command
 * DFS and find path in a graph !
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX 21
//3 dataStructre for defination of the graph

typedef struct edge edge_t;
typedef struct node{

	// what ever we have in the node
	char *name;
	struct node *next;
	struct edge_t *edges;
	int traversed;
}node_t;

typedef struct edge{
	int weight;
	struct edge *next;
	node_t *vertex;
}edge_t;

typedef struct graph{
	node_t * head_graph;
}graph_t;

typedef struct path{

	char *name;
	struct path *next;
}path_t;

/*prototypes*/

graph_t *readFile(char *);
node_t *search(node_t *,char *);
node_t *create_node(node_t **,char *); // you need 2 pointers!!
edge_t *create_edge(edge_t *,node_t *,int);
void graph_write(graph_t *);
void find_path(node_t *,char *,char *,int , int , char *);
int Dfs_modified(node_t *,node_t *,path_t *,path_t **,int ,int ,int,int,int ,int );
path_t *list_dispose(path_t *);
void list_visit(path_t *,FILE *);
path_t *list_copy(path_t *,path_t *);
path_t *push(path_t *,char *);
path_t *pop(path_t *);
void graph_dispose(graph_t *);

int main(int argc,char *argv[]){

	graph_t *g;

	char *id1,*id2;
	int k,p;
	char *fileOut;

	if(argc != 7){
		printf("Error on number of parameters\n");
		exit(EXIT_FAILURE);
	}
	id1=argv[2];
	id2=argv[3];
	k=atoi(argv[4]);
	p=atoi(argv[5]);
	fileOut=argv[6];

	g=readFile(argv[1]);
	graph_write(g);

	//find the path with at most k vertices!

	find_path(g->head_graph,id1,id2,k,p,fileOut);

	graph_dispose(g);

	return 0;
}
graph_t *readFile(char *fileName){

	FILE *fp;
	graph_t *g;
	char src[21],dest[21];
	int weight;
	node_t *src_p;
	node_t *dest_p;

	fp=fopen(fileName,"r");

	g=(graph_t *)malloc(sizeof(g));
	if(g==NULL){
		printf("Allocation Error on head of the graph !\n");
		exit(EXIT_FAILURE);
	}
	g->head_graph=NULL;

	if(fp==NULL){
		printf("Error on opening File!\n");
		exit(EXIT_FAILURE);
	}

	while(fscanf(fp,"%s %d %s",src,&weight,dest)!=EOF){
		src_p=search(g->head_graph,src);
		if(src_p==NULL){
			src_p=create_node(&(g->head_graph),src);
		}
		dest_p=search(g->head_graph,dest);
		if(dest_p ==NULL){
			dest_p=create_node(&(g->head_graph),dest);
		}
		src_p->edges=create_edge(src_p->edges,dest_p,weight);
	}
	fclose(fp);
	return g;
}
node_t *search(node_t *head,char *value){
	node_t *curr;
	curr=head;
	while(curr != NULL){
		if(strcmp(curr->name,value)==0){
			return curr;
		}
		curr=curr->next;
	}
return NULL;
}
node_t *create_node(node_t **head,char *value){

	node_t *new_node;
	node_t *tmp;
	new_node=(node_t *)malloc(sizeof(node_t));
	if(new_node==NULL){
		printf("Allocation Error on vertices\n");
		exit(EXIT_FAILURE);
	}
	new_node->name=(char *)malloc((strlen(value)+1)*sizeof(char));
	if(new_node->name==NULL){
		printf("Allocation Error!\n");
		exit(EXIT_FAILURE);
	}
	strcpy(new_node->name,value);
	new_node->edges=NULL;
	new_node->traversed=0;
	tmp=new_node;
	new_node->next=*head;
	return tmp;
}
edge_t *create_edge(edge_t *head,node_t *dest,int w){
	edge_t * new_edge;
	new_edge=(edge_t *)malloc(sizeof(edge_t));
	if(new_edge ==NULL){
		printf("Allocation Error on the edge of the graph\n");
		exit(EXIT_FAILURE);

	}
	new_edge->weight=w;
	new_edge->vertex=dest;
	new_edge->next=head;
	head=new_edge;
	return head;
}

void graph_write(graph_t *g){

	node_t *node_tmp;
	edge_t *edge_tmp;

	printf("Graph=\n");

	node_tmp=g->head_graph;

	while(node_tmp !=NULL){
		printf("%s->",node_tmp->name);
		edge_tmp=node_tmp->edges;
		while(edge_tmp!=NULL){
			printf("%s (%d)",edge_tmp->vertex->name,edge_tmp->weight);
			edge_tmp=edge_tmp->next;
		}
		printf("\n");
		node_tmp=node_tmp->next;
	}
	return ;
}
void find_path(node_t *vertices,char *src,char *dest,int k, int p, char *fileName){

	node_t *src_ptr;
	node_t *dest_ptr;
	path_t *path_curr;
	path_t * path_best;
	int max;
	FILE *fp;

	path_best=NULL;
	path_curr=NULL;

	max=0;
	src_ptr=search(vertices,src);
	dest_ptr=search(vertices,dest);

//path best each time changed so I need to save the address of my path_best everytime !!

	max=Dfs_modified(src_ptr,dest_ptr,path_curr,&path_best,0,0,0,max,k,p);

	printf("satisfying path:\n");
	fp=fopen(fileName,"w");
	if(fp==NULL){
		printf("Opening File Error!\n");
		exit(EXIT_FAILURE);
	}
	list_visit(path_best,fp);
	list_dispose(path_best);
	fclose(fp);



}

//DFS_Visit RECURSION path_best has 2 pointer !!

int Dfs_modified(node_t *src_ptr,node_t *dest_ptr,path_t *path_curr,path_t **path_best,
		int k,int p,int sum_curr,int sum_max,int k_max,int p_max){

	edge_t *loc_edge;
	loc_edge=src_ptr->edges;

	//termination case

	if(src_ptr==dest_ptr){
		if(sum_curr > sum_max){

			sum_max=sum_curr;
			*path_best=list_dispose(*path_best);
			*path_best=push(*path_best,dest_ptr->name);
			*path_best=list_copy(*path_best,path_curr);

		}
		return sum_max;
	}
	//find the MAX

	if(src_ptr->traversed >0){
		if(k>k_max){
			return sum_max;
		}
		if(p+1 > p_max){
			return sum_max;
		}
	}

	p++;
	k++;
	//is like tracking the recursive function

	src_ptr->traversed++;

	//create a new current path
	path_curr=push(path_curr,src_ptr->name);

	//now move in to your path
	while(loc_edge!=NULL){
		sum_max=Dfs_modified(loc_edge->vertex,dest_ptr,path_curr,path_best,k,p,sum_curr+loc_edge->weight,sum_max,k_max,p_max);
		loc_edge=loc_edge->next;
	}
	path_curr=pop(path_curr);
	src_ptr->traversed--;

	return sum_max;


}

//is for getting rid of my list

path_t *list_dispose(path_t *path_head){
	path_t *path_tmp,*path_next;

	path_tmp=path_head;

	while(path_tmp !=NULL){
		path_next=path_tmp->next;
		free(path_tmp->name);
		free(path_tmp);
		path_tmp=path_next;
	}
	return NULL;

}
void list_visit(path_t *head,FILE *fp){

	path_t *curr;
	curr=head;
	while(curr!=NULL){
		printf("%s ",curr->name);
		curr=curr->next;
	}
	printf("\n");
}
path_t *list_copy(path_t *path_best,path_t *path_curr){

	path_t *curr;
	curr=path_curr;

	while(curr != NULL){

		//create a new best path and go to the next path

		path_best=push(path_best,curr->name);
		curr=curr->next;
	}
	return path_best;

}

//insert data in to my list create a node

path_t *push(path_t *head,char *data){

	path_t *new_path;

	new_path=(path_t *)malloc(sizeof(path_t));
	if(new_path==NULL){
		printf("Allocation Error!\n");
		exit(EXIT_FAILURE);
	}
	new_path->name=(char *)malloc(sizeof(strlen(data)+1)*sizeof(char));
	if(new_path->name==NULL){
		printf("Allocation Error 2!\n");
		exit(EXIT_FAILURE);
	}

	strcpy(new_path->name,data);
	new_path->next=head;
	head=new_path;
	return head;
}

path_t *pop(path_t *head){

	path_t *old;
	if(head==NULL){
		printf("list is empty!");

	}else{
		old=head;
		head=head->next;
		free(old);
	}
	return head;
}

void graph_dispose(graph_t *g){

	node_t *node_tmp1,*node_tmp2;
	edge_t *edge_tmp1,*edge_tmp2;
	node_tmp1=g->head_graph;
	while(node_tmp1 !=NULL){
		node_tmp2=node_tmp1;
		free(node_tmp1->name);
		edge_tmp1=node_tmp1->edges;
		while(edge_tmp1!=NULL){
			edge_tmp2=edge_tmp1;
			free(edge_tmp1);
			edge_tmp1=edge_tmp2->next;
		}

		node_tmp1=node_tmp2->next;

	}
	free(g);


}
