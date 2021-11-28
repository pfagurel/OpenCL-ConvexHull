extends Node2D

var draw_circles_queue = []
var draw_ch_queue = []

var t_vec = []
var l = 0

var grid
var margin_size = 10

var draw_ch = false

	
export var circle_radius = 4
export var line_width = 2.0	


class ch_sorting:
	var lowest = Vector2(INF,0)
	
	func odet(a, b, c):
		return a.x * (b.y - c.y) - b.x * (a.y - c.y) + c.x * (a.y - b.y)
		
	func get_lowest(arr):
		for p in arr:
			if p.x < lowest.x:
				lowest = p
				
	func odet_sort(a, b):
		return odet(lowest,a,b) < 0
	
	func xy_sort(a,b):
		return a.x < b.x or ((a.x == b.x) and a.y < b.y)


func _ready():
	grid = $Grid
	


func compute_file():
	var c_sort = ch_sorting.new()
	print("\t\t\t***Reading***\n")
	var v = read_file(9000000)
	print("\t\t\t***Sorting***\n")
	v.sort_custom(c_sort,"xy_sort")

	grid.compute_hull(v,false)

func _input(event):
	if event is InputEventMouseButton and event.is_action_pressed("mouse_left"):
		if event.position.x <= grid.global_position.x - (grid.get_texture().get_size().x*grid.scale.x/2) + margin_size \
		or event.position.x >= grid.global_position.x + (grid.get_texture().get_size().x*grid.scale.x/2) - margin_size \
		or event.position.y <= grid.global_position.y - (grid.get_texture().get_size().y*grid.scale.y/2) + margin_size \
		or event.position.y >= grid.global_position.y + (grid.get_texture().get_size().y*grid.scale.y/2) - margin_size :
			return
			
		draw_circles_queue.append(event.position)
		update()
		
	if event is InputEventKey and event.is_action_pressed("ui_right"):
		if l < len(t_vec)-1:
			draw_ch_queue = t_vec[l+1]
			l += 1
			draw_ch = true
			update()
	elif event is InputEventKey and event.is_action_pressed("ui_left"):
		if l > 0:
			draw_ch_queue = t_vec[l-1]
			l -= 1
			draw_ch = true
			update()
			
func _draw():
	for i in range(len(draw_circles_queue)):
		draw_circle(draw_circles_queue[i],circle_radius,Color.black)
		
	if draw_ch:
		for i in range(len(draw_ch_queue) ):
		  draw_line(draw_ch_queue[i], draw_ch_queue[(i + 1) % (len(draw_ch_queue))],Color.black,line_width,true)
	  
func add_step(var arr:Array):
	t_vec.append(arr)



			
func read_file(max_points = INF):
	var points = []
	var next
	var file = File.new()
	file.open("res://data/10000000.txt", File.READ)
	var i =0
	while i < max_points:
		next = file.get_line().split(" ")

		if !file.eof_reached():
			points.append(Vector2(next[0],next[1]))
		else:
			break		
		i+=1
		
	file.close()
	return points
		


func _on_Compute_pressed():
	t_vec.clear();
	l=-1
	
	var c_sort = ch_sorting.new()
	print("\t\t\t***xySorting***\n")
	draw_circles_queue.sort_custom(c_sort,"xy_sort")
	
	var result = grid.compute_hull(draw_circles_queue,true)

	var non_duplicate = []
	for i in range(len(result)):
		if !result[i] in non_duplicate:
			non_duplicate.append(result[i])

	draw_ch_queue = non_duplicate	
	
	c_sort.get_lowest(draw_ch_queue)
	draw_ch_queue.erase(c_sort.lowest)
	draw_ch_queue.sort_custom(c_sort,"odet_sort")
	draw_ch_queue.push_front(c_sort.lowest)		
	draw_ch = true
	update()
