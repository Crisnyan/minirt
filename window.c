//#define HEIGHT 1080
//#define WIDTH 1920
//#define HEIGHT 720
//#define WIDTH 1280
/*#define HEIGHT 3*/
/*#define WIDTH 3*/
/*#define HEIGHT 101*/
/*#define WIDTH 101*/
/*#define HEIGHT 200*/
/*#define WIDTH 200*/
#define HEIGHT 501
#define WIDTH 501
/*#define HEIGHT 1001*/
/*#define WIDTH 1001*/
//#define HEIGHT 2001
//#define WIDTH 2001

#define PIXEL 1
#define RGB 1
#define RAY 0
#define PLANE 1

//#define DELIM 0.00001f
#define SPHERE_TAG 0
#define PLANE_TAG 1
#define CYLINDER_TAG 2

#define BASE 0
#define VERT 1
#define HZN 2
#define BOTH 3

#define ESC 65307
#define W_KEY 119
#define A_KEY 97
#define S_KEY 115
#define D_KEY 100

#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "mlx/mlx.h"
#include "mlx/mlx_int.h"
#include "libft/libft.h"

typedef struct s_image
{
	void	*img;
	char	*addr;
	int		bpp;
	int		linelen;
	int		endian;
}	t_image;

typedef	struct s_vec3
{
	float	x;
	float	y;
	float	z;
}	t_vec3;

typedef	struct s_mat3
{
	t_vec3	c1;
	t_vec3	c2;
	t_vec3	c3;
}	t_mat3;

typedef struct	s_ray
{
	t_vec3	dir;
	t_vec3	origin;
	float	t;
}	t_ray;

typedef struct	s_sphere
{
	float	diameter;	
}	t_sphere;

typedef struct	s_plane
{
	t_vec3	dir;
}	t_plane;

typedef struct	s_cylinder
{
	t_vec3	dir;
	float	diameter;	
	float	height;	
}	t_cylinder;

typedef union s_figure
{
	t_cylinder	cylinder;
	t_plane		plane;
	t_sphere	sphere;
}	t_figure;

typedef struct s_amblight
{
	float	intensity;
	int		rgb;
} t_amblight;

typedef struct s_camera
{
	t_vec3	pos;
	t_vec3	forward;
	t_vec3	right;
	t_vec3	up;
	int		fov;
}	t_camera;

typedef struct s_light
{
	t_vec3			pos;
	float			intensity;
	int				rgb;
}	t_light;

typedef struct s_shape
{
	//TODO:	figure should contain a funtion that describes the surface of the figure, i.e. x^2+y^2=r^2
	t_figure		figure;	
	t_vec3			pos;
	struct s_shape	*next;
	int				rgb;
	char			shape_tag;
}	t_shape;

typedef struct s_scene
{
	t_amblight	ambient;
	t_camera	camera;
	t_light		light;
	t_shape		*headshape;
	char		init;
}	t_scene;

typedef struct s_vars
{
	void		*mlx;
	t_win_list	*win;
	t_image		img;
	t_shape		*rcurrent;
	t_shape		*lcurrent;
	int			x_pix;
	int			y_pix;
	char		section;
}	t_vars;


int	close_win(t_vars *vars)
{
	mlx_destroy_window(vars->mlx, vars->win);
	exit(0);
}

int	key_press(int keycode, t_vars *vars)
{
	(void)vars;
	printf("the key pressed has a keycode of %d\n", keycode);
	return (0);
}

int	esc_press(int keycode, t_vars *vars)
{
	if (keycode == ESC)
		close_win(vars);
	return (0);
}

void key_hooks(t_vars *vars)
{
	mlx_key_hook(vars->win, esc_press, vars);
}

int is_space(char c)
{
	if (c == ' ' || (c >= 7 && c <= 13))
		return (1);
	return (0);
}

int is_digit(char digit)
{
	if (digit >= '0' && digit <= '9')
		return (1);
	return (0);
}

int	is_break(char *str)
{
	if (str == NULL)
		return (0);
	if (!*str || !(*str + 1))
		return (0);
	//printf("before if str is: %s\n", str);
	if (!strncmp(str, "A", 1) || !strncmp(str, "C", 1) || !strncmp(str, "L", 1)
		|| !strncmp(str, "sp", 1) || !strncmp(str, "pl", 2) || !strncmp(str, "cy", 2))
		return (1);
	return (0);
}

char	*skip_space(char *const line)
{
	char *slice;

	slice = line;
	while (*slice && is_space(*slice))
		 slice++;
	return (slice);
}

void	printvec(t_vec3 vec)
{
	(void)vec;
	printf("x: %.7f\n", vec.x);
	printf("y: %.7f\n", vec.y);
	printf("z: %.7f\n", vec.z);
	printf("(%.3f,%.3f,%.3f)\n", vec.x, vec.y, vec.z);
}

//void	printlights(t_scene *scene)
//{
//	int	i;
//
//	i = 1;
//	t_light *ptr;
//	ptr = scene->light;
//	while (ptr)
//	{
//		printf("light %d\n", i);
//		printf("light pos.x: %.1f\n", ptr->pos.x);
//		printf("light pos.y: %.1f\n", ptr->pos.y);
//		printf("light pos.z: %.1f\n", ptr->pos.z);
//		printf("light intensity: %.1f\n", ptr->intensity);
//		printf("light red value: %d\n", (ptr->rgb >> 16) & 255);
//		printf("light green value: %d\n", (ptr->rgb >> 8) & 255);
//		printf("light blue value: %d\n",  (ptr->rgb) & 255);
//		ptr = ptr->next;
//		i++;
//	}
//}

void	printshapes(t_scene *scene)
{
	int	i;

	i = 1;
	t_shape *ptr;

	ptr = scene->headshape;
	while (ptr)
	{
		printf("shape %d\n", i);
		printf("shape x position: %.1f\n", ptr->pos.x);
		printf("shape y position: %.1f\n", ptr->pos.y);
		printf("shape z position: %.1f\n", ptr->pos.z);
		if (ptr->shape_tag == SPHERE_TAG)
			printf("sphere diameter: %.1f\n", ptr->figure.sphere.diameter);
		if (ptr->shape_tag == PLANE_TAG)
		{
			printf("plane normal x: %.1f\n", ptr->figure.plane.dir.x);
			printf("plane normal y: %.1f\n", ptr->figure.plane.dir.y);
			printf("plane normal z: %.1f\n", ptr->figure.plane.dir.z);
		}
		if (ptr->shape_tag == CYLINDER_TAG)
		{
			printf("cylinder x direction: %.1f\n", ptr->figure.cylinder.dir.x);
			printf("cylinder y direction: %.1f\n", ptr->figure.cylinder.dir.y);
			printf("cylinder x direction: %.1f\n", ptr->figure.cylinder.dir.z);
			printf("cylinder diameter: %.1f\n", ptr->figure.cylinder.diameter);
			printf("cylinder height: %.1f\n", ptr->figure.cylinder.height);
		}
		printf("shape red value: %d\n", (ptr->rgb >> 16) & 255);
		printf("shape green value: %d\n", (ptr->rgb >> 8) & 255);
		printf("shape blue value: %d\n",  (ptr->rgb) & 255);
		ptr = ptr->next;
		i++;
	}
	
}

char *get_whole(char **str, float *whole, int *neg)
{
	int res;
	char *first;

	res = 0;
	first = *str;
	while (**str && !is_space(**str) && **str != '.' && **str != ',')
	{
		//printf("get_whole in\n");
		if (*str == first && **str == '-')
			*neg = -1;
		else if (!is_digit(**str))
			return (NULL);
		else if (is_digit(**str))
			res = res * 10 + (**str - '0');
		(*str)++;
	}
	//printf("get_whole out\n");
	*whole = res;
	return (*str);
}

char *get_decimal(char **str, float *decimal, int *counter)
{
	*decimal = 0;
	*counter = 0;
	if (**str == '.')
		(*str)++;
	while (**str && !is_space(**str) && **str != ',')
	{
		//printf("get_decimal in\n");
		if (!is_digit(**str))
			return (NULL);
		*decimal = *decimal * 10 + (**str - '0');
		//printf("decimal is: %.1f\n", *decimal);
		(*counter)++;
		(*str)++;
	}
	//printf("get_decimal out\n");
	//printf("counter is: %.1f\n", *decimal);
	return (*str);
}

float	pow_counter(int counter)
{
	float	res;

	if (counter == 0)
		return (0);
	res = 1.0f;
	while (counter--)
		res *= 0.1f;
	return (res);
}

char	*ft_atof(char **str, float *num)
{
	float whole;
	float decimal;
	int counter;
	int neg;
	
	counter = 0;
	neg = 1;
	//printf("slice before is: %s", *str);
	if (get_whole(str, &whole, &neg) == NULL)
		return (NULL);
	//printf("atof whole side is: %.1f\n", whole);
	//printf("slice after whole is: %s", *str);
	if (get_decimal(str, &decimal, &counter) ==  NULL)
		return (NULL);
	//printf("atof decimal side is: %.1f\n", decimal);
	//printf("slice after decimal is: %s", *str);
	*num = neg * ((float)whole + (float)decimal * pow_counter(counter));
	//printf("atof final number is: %.1f\n", *num);

	return (*str);
}

char	*check_neg(char **str)
{

	//printf("enters check_neg with slice:%s\n", *str);
	(*str)++;
	//printf("slice after pointer mov:%s\n", *str);
	if (!(*str) || is_space(**str))
		return (NULL);
	while (**str)
	{
		//printf("slice before loop:%s|\n", *str);
		if (**str != '0' && !is_space(**str))
			return (NULL);
		//printf("slice after loop:%s|\n", *str);
		(*str)++;
		if (**str && **str == ',')
			break ;
	}
	//printf("exits check_neg with slice:%s\n", *str);
	return (*str);
}

char	*get_byte(char **str, int *num)
{
	*num = 0;
	//printf("get_byte in\n");
	if (**str == '-' && check_neg(str) == NULL)
		return (NULL);
	//printf("slice after negative check:%s\n", *str);
	while (**str && **str != ',' && !is_space(**str))
	{
		if (!is_digit(**str))
			return (NULL);
		*num = *num * 10 + (**str - '0');
		//printf("get_byte num is: %d\n", *num);
		(*str)++;
		//printf("str is: %s|\n", *str);
	if (abs(*num) > 255)
		return (NULL);
	}
	//printf("get_byte out\n");
	return (*str);
}

char	*bitoi(char **str, int *rgb, char c)
{
	int	num;

	//printf("entra en bitoi\n");
	if (get_byte(str, &num) == NULL)
		return (NULL);
	if (c == 't')
		*rgb |= (num << 24);
	else if (c == 'r')
		*rgb |= (num << 16);
	else if (c == 'g')
		*rgb |= (num << 8);
	else if (c == 'b')
		*rgb |= num;
	//printf("sale de bitoi\n");
	return (*str);
}

t_vec3	quadratic(float a, float b, float c)
{
	t_vec3 value;

	value.x = b * b - 4 * a * c;
	if (value.x < 0)
	{
		//printf("has no roots\n");
		value.x = 0;
		return (value);
	}
	value.y = (-b + sqrtf(value.x)) / (2 * a);
	if (value.x == 0)
	{
		//printf("has only one root\n");
		//printf("root is: %.7f\n", value.y);
		value.x = 1;
		return (value);
	}
	else 
	{
		value.z = (-b - sqrtf(value.x)) / (2 * a);
		//printf("has two roots\n");
		//printf("root with positive determinant is: %.7f\n", value.y);
		//printf("root with negative determinant is: %.7f\n", value.z);
		value.x = 2;
	return (value);
	}
}

char	*get_vec(char *slice, t_vec3 *pos)
{
	slice = skip_space(slice);
	if (ft_atof(&slice, &pos->x) == NULL)
		return (NULL);
	if (*slice && *(slice++) != ',')
		return (NULL);
	if (ft_atof(&slice, &pos->y) == NULL)
		return (NULL);
	//printf("slice is: %s", slice);
	if (*slice && *(slice++) != ',')
		return (NULL);
	//printf("slice is: %s", slice);
	if (ft_atof(&slice, &pos->z) == NULL)
		return (NULL);
	//printf("exits get_vec successfully\n");
	return (slice);
}

float	vec_length(t_vec3 vec)
{
	return (sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

int	norm(t_vec3 *dir)
{
	float	length;
	
	//printf("non normalized vector is: %.1f, %.1f, %.1f\n", dir->x, dir->y, dir->z);
	length = sqrtf(dir->x * dir->x + dir->y * dir->y + dir->z * dir->z);
	//printf("the length is: %.4f\n", length);
	if (length == 0)
		return (0);
	dir->x /= length;
	dir->y /= length;
	dir->z /= length;
	//printf("normalized vector is: %.4f, %.4f, %.4f\n", dir->x, dir->y, dir->z);
	return (1);
}

char	*get_rgb(char *slice, int *rgb)
{
	//printf("slice before skip space: %s\n", slice);
	slice = skip_space(slice);
	*rgb = 0;
	//printf("slice before first bitoi: %s\n", slice);
	if (bitoi(&slice, rgb, 'r') == NULL)
		return (NULL);
	//printf("slice after first bitoi: %s\n", slice);
	if (*slice && *(slice++) != ',')
		return (NULL);
	//printf("slice before second bitoi: %s\n", slice);
	if (bitoi(&slice, rgb, 'g') == NULL)
		return (NULL);
	//printf("slice after second bitoi: %s\n", slice);
	if (*slice && *(slice++) != ',')
		return (NULL);
	//printf("slice before third bitoi: %s\n", slice);
	if (bitoi(&slice, rgb, 'b') == NULL)
		return (NULL);
	*rgb |= (0 << 24);
	printf("red value is: %d\n", *rgb >> 16);
	printf("green value is: %d\n", (*rgb >> 8) & 255);
	printf("blue value is: %d\n", *rgb & 255);
	return (slice);
}

char	*get_intensity(char *slice, float *intensity)
{
	slice = skip_space(slice);
	//printf("slice is intensity: %s", slice);
	if (ft_atof(&slice, intensity) == NULL)
		return (NULL);
	//printf("slice is intensity: %s", slice);
	if (*intensity < 0.0f || *intensity > 1)
		return (NULL);
	return (slice);
}

char	*get_fov(char *slice, int *fov)
{
	slice = skip_space(slice);
	if (get_byte(&slice, fov) == NULL)
		return (NULL);
	if (*fov <= 0 || *fov > 180)
		return (NULL);
	return (slice);
}

char	*get_parameter(char *slice, float *to_get)
{
	slice = skip_space(slice);
	if (ft_atof(&slice, to_get) == NULL)
		return (NULL);
	return (slice);
}

//int	create_lnode(t_light **last_light)
//{ 
//	//printf("enters create_node with last_light: %p\n", *last_light);
//	*last_light = malloc(sizeof(t_light));
//	//printf("create_node makes last_light: %p\n", *last_light);
//	if (!(*last_light))
//		return (0);
//	(*last_light)->rgb = (0 << 24) | (255 << 16) | (255 << 8) | 255;
//	(*last_light)->next = NULL;
//	return (1);
//}

int	create_snode(t_shape **last_shape)
{
	//printf("enters create_node with last_shape: %p\n", *last_shape);
	*last_shape = malloc(sizeof(t_shape));
	//printf("create_node makes last_shape: %p\n", *last_shape);
	if (!(*last_shape))
		return (0);
	(*last_shape)->next = NULL;
	//printf("create_node exits successfully\n");
	return (1);
}

int	is_null_vec(t_vec3 vec)
{
	if (vec.x || vec.y || vec.z)
		return (0);
	return (1);
}

t_vec3	scale_vec(float s, t_vec3 v)
{
	//printf("scalar s: %.3f\n", s);
	//printf("vec v:\n");
	//printvec(v);
	return ((t_vec3){s * v.x, s * v.y, s * v.z});
}

t_vec3	sum_vec(t_vec3 u, t_vec3 v)
{
	t_vec3	vec;

	//printf("vec u:\n");
	//printvec(u);
	//printf("vec v:\n");
	//printvec(v);
	vec.x = u.x + v.x;
	vec.y = u.y + v.y;
	vec.z = u.z + v.z;
	return (vec);
}

t_vec3	rhcross_product(t_vec3 u, t_vec3 v)
{
	t_vec3	vec;

	vec.x = u.y * v.z - u.z * v.y;
	vec.y = u.z * v.x - u.x * v.z;
	vec.z = u.x * v.y - u.y * v.x;
	return (vec);
}

t_vec3	lhcross_product(t_vec3 u, t_vec3 v)
{
	t_vec3	vec;

	vec.x = u.z * v.y - u.y * v.z;
	vec.y = u.x * v.z - u.z * v.x;
	vec.z = u.y * v.x - u.x * v.y;
	return (vec);
}

float	dot_product(t_vec3 u, t_vec3 v)
{
	return (u.x * v.x + u.y * v.y + u.z * v.z);
}

t_vec3	make_vec(float x, float y, float z)
{
	t_vec3	vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;
	return (vec);
}

t_ray	make_ray(t_vec3 o, t_vec3 d)
{
	t_ray	ray;

	ray.origin = o;
	ray.dir = d;
	return (ray);
}

t_mat3	make_mat(t_vec3 x, t_vec3 y, t_vec3 z)
{
	t_mat3	mat;

	mat.c1 = x;
	mat.c2 = y;
	mat.c3 = z;
	return (mat);
}

void	apply_transform(t_vec3 *vec, t_mat3 *mat)
{
	vec->x = vec->x * mat->c1.x + vec->y * mat->c2.x + vec->z * mat->c3.x;
	vec->x = vec->x * mat->c1.y + vec->y * mat->c2.y + vec->z * mat->c3.y;
	vec->x = vec->x * mat->c1.z + vec->y * mat->c2.z + vec->z * mat->c3.z;
}

t_vec3	vec_rotate_x(t_vec3 vec, float angle)
{
	//NOTE: Rotations are clockwise (left-hand)
	t_mat3	mat;
	t_vec3	rotated;

	rotated = vec;
	mat = make_mat(make_vec(1, 0, 0),
					make_vec(0, cosf(angle), -sinf(angle)),
					make_vec(0, sinf(angle), cosf(angle)));
	apply_transform(&rotated, &mat);
	return(rotated);
}

t_vec3	vec_rotate_y(t_vec3 vec, float angle)
{
	//NOTE: Rotations are clockwise (left-hand)
	t_mat3	mat;
	t_vec3	rotated;

	rotated = vec;
	mat = make_mat(make_vec(cosf(angle), 0, -sinf(angle)),
					make_vec(0, 1, 0),
					make_vec(sinf(angle), 0, cosf(angle)));
	apply_transform(&rotated, &mat);
	return(rotated);

}

t_vec3	vec_rotate_z(t_vec3 vec, float angle)
{
	//NOTE: Rotations are clockwise (left-hand)
	t_mat3	mat;
	t_vec3	rotated;

	rotated = vec;
	mat = make_mat(make_vec(cosf(angle), -sinf(angle), 0),
					make_vec(sinf(angle), cosf(angle), 0),
					make_vec(0, 0, 1));
	apply_transform(&rotated, &mat);
	return(rotated);
}

t_vec3	check_unidim_r(t_vec3 cam_dir)
{
	if (cam_dir.x == 1)
		return ((t_vec3){0, 0, 1});
	else if (cam_dir.y == 1 || cam_dir.y == -1)
		return ((t_vec3){1, 0, 0});
	else if (cam_dir.z == 1)
		return ((t_vec3){-1, 0, 0});
	else if (cam_dir.x == -1)
		return ((t_vec3){0, 0, -1});
	else if (cam_dir.z == -1)
		return ((t_vec3){1, 0, 0});
	return ((t_vec3){0, 0, 0});
}

t_vec3	check_bidim_r(t_vec3 cam_dir)
{
	if (!cam_dir.x)
	{
		if (cam_dir.z > 0)
			return ((t_vec3){-1, 0, 0});
		else
			return ((t_vec3){1, 0, 0});
	}
	else if (!cam_dir.y)
			return ((t_vec3)vec_rotate_y(cam_dir, M_PI_2));
	else if (!cam_dir.z)
	{
		if (cam_dir.x > 0)
			return ((t_vec3){0, 0, -1});
		else
			return ((t_vec3){1, 0, 0});
	}
	return ((t_vec3){0, 0, 0});
}

t_vec3	check_unidim_u(t_vec3 cam_dir)
{
	if (fabsf(cam_dir.x) == 1 || fabsf(cam_dir.z) == 1)
		return ((t_vec3){0, 1, 0});
	else if (cam_dir.y == 1)
		return ((t_vec3){0, 0, 1});
	else if (cam_dir.y == -1)
		return ((t_vec3){0, 0, -1});
	return ((t_vec3){0, 0, 0});
}

t_vec3	check_bidim_u(t_vec3 cam_dir)
{
	if (!cam_dir.y)
			return ((t_vec3){0, 1, 0});
	else if (!cam_dir.x)
	{
		if (cam_dir.z > 0)
			return ((t_vec3)vec_rotate_x(cam_dir, M_PI_2));
		else
			return ((t_vec3)vec_rotate_x(cam_dir, -M_PI_2));
	}
	else if (!cam_dir.z)
	{
		if (cam_dir.x > 0)
			return ((t_vec3)vec_rotate_z(cam_dir, -M_PI_2));
		else
			return ((t_vec3)vec_rotate_z(cam_dir, M_PI_2));
	}
	return ((t_vec3){0, 0, 0});
}

t_vec3	get_right(t_vec3 cam_dir)
{
	t_vec3	vec;

	// FIX: Camera is broken for non-trivial directions
	vec = check_unidim_r(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	vec = check_bidim_r(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	vec.y = 0;
	if ((cam_dir.x > 0 && cam_dir.z > 0) || (cam_dir.x < 0 && cam_dir.z > 0))
	{
		vec.x = -cam_dir.z;
		vec.z = cam_dir.x;
	}
	else
	{
		vec.x = cam_dir.z;
		vec.z = -cam_dir.x;
	}
	norm(&vec);
	return (vec);
}

t_vec3	get_up(t_vec3 cam_dir, t_vec3 right)
{
	t_vec3	vec;

	vec = check_unidim_u(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	/*vec = check_bidim_u(cam_dir);*/
	/*if (!is_null_vec(vec))*/
	/*	return (vec);*/
	vec = lhcross_product(cam_dir, right);
	norm(&vec);
	return (vec);
}

//float sphere_surface(t_vec3 v, float d)
//{
//	return (v.x * v.x + v.y * v.y + v.z * v.z - 0.5f * d);
//}
//
//float plane_surface(t_vec3 v)
//{
//	return (v.x * v.x + v.y * v.y + v.z * v.z - 0.5f * d);
//}
//
//float cylinder_surface(t_vec3 v, )
//{
//	return (v.x * v.x + v.y * v.y + v.z * v.z - 0.5f * d);
//}

int	set_amblight(char *slice, t_scene *scene, char *is_set)
{
	//printf("enters set_amblight\n");
	if (*is_set & 1)
		 return (0);
	//printf("enters get_intensity\n");
	slice = get_intensity(slice, &scene->ambient.intensity);
	//printf("exits get_intensity\n");
	if (slice == NULL)
		return (0);
	//printf("enters get_rgb\n");
	//printf("slice is: %s", slice);
	slice = get_rgb(slice, &scene->ambient.rgb);
	//printf("slice is: %s", slice);
	//printf("exits get_rgb\n");
	if (slice == NULL)
		return (0);
	//printf("enters check_break\n");
	//printf("slice is: %s", slice);
//	while (*slice)
//		if (!is_space((*slice)++))
//		{
			//printf("slice is: %s", slice);
//			return (0);
//		}
	//printf("passes check_break\n");
	*is_set += 1;
	printf("amblight intensity: %.1f\n", scene->ambient.intensity);
	printf("amblight red value: %d\n", (scene->ambient.rgb >> 16) & 255);
	printf("amblight green value: %d\n", (scene->ambient.rgb >> 8) & 255);
	printf("amblight blue value: %d\n",scene->ambient.rgb & 255);
	//printf("exits set_amblight\n");
	return (1);
}

int	set_camera(char *slice, t_scene *scene, char *is_set)
{
	//printf("enters set_camera\n");
	if (*is_set >> 1 & 1)
		 return (0);
	slice = get_vec(slice, &scene->camera.pos);
	if (slice == NULL)
		return (0);
	slice = get_vec(slice, &scene->camera.forward);
	if (slice == NULL)
		return (0);
	if (!norm(&scene->camera.forward))
		return (0);
	slice = get_fov(slice, &scene->camera.fov);
	if (slice == NULL)
		return (0);
	*is_set += 2;
	scene->camera.right = get_right(scene->camera.forward);
	scene->camera.up = get_up(scene->camera.forward, scene->camera.right);
	printf("camera pos\n");
	printvec(scene->camera.pos);
	printf("camera forward\n");
	printvec(scene->camera.forward);
	printf("camera right\n");
	printvec(scene->camera.right);
	printf("camera up\n");
	printvec(scene->camera.up);
	printf("camera fov: %d\n", scene->camera.fov);
	//printf("exits set_camera\n");
	return (1);
}

int	set_light(char *slice, t_scene *scene, char *is_set)
{
	//static t_light	*last_light = NULL;
	
	//printf("\nenters with light: %p\n\n", scene->light);
	//printf("enters set_light\n");
	if (*is_set >> 2)
		 return (0);
	slice = get_vec(slice, &scene->light.pos);
	if (slice == NULL)
		return (0);
	slice = get_intensity(slice, &scene->light.intensity);
	if (slice == NULL)
		return (0);
	printf("slice is: %s", slice);
	while (*slice)
	{
		if (!is_space(*slice))
		{
			slice = get_rgb(slice, &scene->light.rgb);
			//printf("slice is: %s", slice);
			break;
		}
		slice++;
	}
	if (slice == NULL)
		return (0);
	*is_set += 4;
	//printf("enters if with last_light: %p\n", last_light);
	//printf("exits if with last_light: %p\n", last_light);
	//printf("\nexits with light: %p\n\n", scene->light);
	return (1);
}


//t_vec3	sphere(t_ray ray)
//{
//	t_vec3 vec;
//
//	vec.x = (ray.origin.x + ray.dir.x) * (ray.origin.x + ray.dir.x);
//	vec.y = (ray.origin.y + ray.dir.y) * (ray.origin.y + ray.dir.y);
//	vec.z = (ray.origin.z + ray.dir.z) * (ray.origin.z + ray.dir.z);
//	return (vec);
//}

float	intersect_sphere(t_ray r, t_vec3 o, float radius)
{
//	t_vec3	vec;
	t_vec3	q_vals;
	t_vec3	abc;
	t_vec3	d;

//	vec = sphere(r);
	d.x = r.origin.x - o.x;
	d.y = r.origin.y - o.y;
	d.z = r.origin.z - o.z;
	abc.x = r.dir.x * r.dir.x + r.dir.y * r.dir.y + r.dir.z * r.dir.z;
	abc.y = 2 * (r.dir.x * d.x + r.dir.y * d.y + r.dir.z * d.z);
	abc.z = d.x * d.x + d.y * d.y + d.z * d.z - radius * radius;
	if (HEIGHT < 200)
	{
		printf("enters intersect sphere\n");
		printf("a is: %.3f\n", abc.x);
		printf("b is: %.3f\n", abc.y);
		printf("c is: %.3f\n", abc.z);
	}
	q_vals = quadratic(abc.x, abc.y, abc.z);
	if (q_vals.x == 0 || q_vals.y < 0)
		return (0);
	else if (q_vals.x == 1)
		return (q_vals.y);
	if (q_vals.z >= 0)
		return (q_vals.z);
	return (q_vals.y);
}

float	intersect_plane(t_ray r, t_vec3 p, t_vec3 pl)
{
	t_vec3	o;
	float	d;
	float	den;
	float	abc;
	float	res;

	if (HEIGHT < 200)
		printf("enters intersect plane\n");
	o.x = r.origin.x;
	o.y = r.origin.y;
	o.z = r.origin.z;
	d = (pl.x * p.x + pl.y * p.y + pl.z * p.z);
	den = d - (pl.x * o.x + pl.y * o.y + pl.z * o.z);
	abc = (pl.x * r.dir.x + pl.y * r.dir.y + pl.z * r.dir.z);
	res = den / abc;
	//if (d + (o.x + o.y + o.z) == 0 && abc == 0)
		//return (0.001);
	if (HEIGHT < 200)
	{
		printf("den is %.3f\n", den);
		printvec(o);
		printf("den is %.3f\n", den);
		printf("abc is %.3f\n", abc);
		printf("res is %.3f\n", res);
		printf("A: %.3f\nB: %.3f\nC: %.3f\nD: %.3f\n", pl.x, pl.y, pl.z, -d);
	}
	if (isnan(res))
		return (0.001);
	else if (res < 0)
	{
		if (HEIGHT < 200)
			printf("enters res < 0\n");
		return (0);
	}
	return (res);
}

//float	light_intersect_sphere(t_ray *r, t_vec3 o, float radius)
//{
////	t_vec3	vec;
//	t_vec3	q_vals;
//	t_vec3	abc;
//	t_vec3	d;
//
////	vec = sphere(r);
//	//printf("enters intersect sphere\n");
//	d.x = r->origin.x - o.x;
//	d.y = r->origin.y - o.y;
//	d.z = r->origin.z - o.z;
//	abc.x = r->dir.x * r->dir.x + r->dir.y * r->dir.y + r->dir.z * r->dir.z;
//	abc.y = 2 * (r->dir.x * d.x + r->dir.y * d.y + r->dir.z * d.z);
//	abc.z = d.x * d.x + d.y * d.y + d.z * d.z - radius * radius;
//	//printf("a is: %.3f\n", abc.x);
//	//printf("b is: %.3f\n", abc.y);
//	//printf("c is: %.3f\n", abc.z);
//	q_vals = quadratic(abc.x, abc.y, abc.z);
//	if (q_vals.x == 0 || q_vals.y < 0)
//		return (0);
//	else if (q_vals.x == 1)
//		return (q_vals.y);
//	if (q_vals.y >= 0)
//		return (q_vals.y);
//	if (q_vals.z >= 0)
//		return (q_vals.z);
//	return (0);
//}

//void	check_light_intersect(t_ray *ray, t_shape *head)
//{
//	// TODO: finish plane and cylinder intersects
//	if (head->shape_tag == SPHERE_TAG)
//		ray->t = light_intersect_sphere(ray, head->pos, head->figure.sphere.diameter / 2);
	//else if (head->shape_tag == PLANE_TAG)
	//	t = intersect_plane(ray, head->pos, head->figure.plane);
	//else
	//	t = intersect_cylinder(ray, head->pos, head->figure.cylinder);
	//		printf ("t is: %.7f\n", t);
		//if (head->shape_tag == PLANE_TAG)
			//t = intersect_plane(ray, head->pos, head->figure.plane.dir / 2);
	//		printf ("t is: %.7f\n", t);
//}

int	set_sphere(char *slice, t_scene *scene, t_shape **last_shape)
{
	//printf("enters set_sphere\n");
	//printf("\nenters with light: %p\n\n", scene->light);
	if (*last_shape != NULL)
	{
		//printf("enters if in set_sphere\n");
		if (!create_snode(&(*last_shape)->next))
			return (0);
		*last_shape = (*last_shape)->next;
	}
	else 
	{
		//printf("enters else in set_sphere\n");
		if (!create_snode(&scene->headshape))
			return (0);
		*last_shape = scene->headshape;
	}
	//printf("passes conitionals in set_sphere\n");
	slice = get_vec(slice + 2, &(*last_shape)->pos);
	if (slice == NULL)
		return (0);
	slice = get_parameter(slice, &(*last_shape)->figure.sphere.diameter);
	if (slice == NULL)
		return (0);
	slice = get_rgb(slice, &(*last_shape)->rgb);
	if (slice == NULL)
		return (0);
	//&(*last_shape)->figure.sphere.create_sphere = ;
	//(*last_shape)->figure.sphere.create_sphere = sphere_surface;
	(*last_shape)->shape_tag = SPHERE_TAG;
	//printf("exits set_sphere\n");
	return (1);
}

int	set_plane(char *slice, t_scene *scene, t_shape **last_shape)
{
	//printf("enters set_plane\n");
	if (*last_shape != NULL)
	{
		if (!create_snode(&(*last_shape)->next))
			return (0);
		*last_shape = (*last_shape)->next;
	}
	else 
	{
		if (!create_snode(&scene->headshape))
			return (0);
		*last_shape = scene->headshape;
	}
	//printf("enters get_vec\n");
	slice = get_vec(slice + 2, &(*last_shape)->pos);
	if (slice == NULL)
		return (0);
	//printf("enters get_vec\n");
	slice = get_vec(slice, &(*last_shape)->figure.plane.dir);
	if (slice == NULL)
		return (0);
	//printf("enters norm\n");
	if (!norm(&(*last_shape)->figure.plane.dir))
		return (0);
	//printf("enters get_rgb\n");
	slice = get_rgb(slice, &(*last_shape)->rgb);
	if (slice == NULL)
		return (0);
	(*last_shape)->shape_tag = PLANE_TAG;
	//printf("exits set_plane\n");
	return (1);
}

int	set_cylinder(char *slice, t_scene *scene, t_shape **last_shape)
{
	//printf("enters set_cylinder\n");
	if (*last_shape != NULL)
	{
		if (!create_snode(&(*last_shape)->next))
			return (0);
		*last_shape = (*last_shape)->next;
	}
	else 
	{
		if (!create_snode(&scene->headshape))
			return (0);
		*last_shape = scene->headshape;
	}
	slice = get_vec(slice + 2, &(*last_shape)->pos);
	if (slice == NULL)
		return (0);
	slice = get_vec(slice, &(*last_shape)->figure.cylinder.dir);
	if (slice == NULL)
		return (0);
	if (!norm(&(*last_shape)->figure.cylinder.dir))
		return (0);
	slice = get_parameter(slice, &(*last_shape)->figure.cylinder.diameter);
	if (slice == NULL)
		return (0);
	slice = get_parameter(slice, &(*last_shape)->figure.cylinder.height);
	if (slice == NULL)
		return (0);
	slice = get_rgb(slice, &(*last_shape)->rgb);
	if (slice == NULL)
		return (0);
	(*last_shape)->shape_tag = CYLINDER_TAG;
	//printf("exits set_cylinder\n");
	return (1);
}

int create_basics(char *slice, t_scene *scene, char * const line, char *file)
{
	static char		is_set = 0;

//	printf("----------------------\nenters with last_light: %p\n", last_light);
	if (*slice == 'A' && !set_amblight(++slice, scene, &is_set))
	{
		//exit((free(line), 1));
		free(line);
		free(file);
		exit(write(2, "Error\nAmbient light couldn't be set\n", 37));
	}
	else if (*slice == 'L' && !set_light(++slice, scene, &is_set))
	{
		free(line);
		free(file);
		exit(write(2, "Error\nLight couldn't be set\n", 29));
	}
	else if (*slice == 'C' && !set_camera(++slice, scene, &is_set))
	{
		free(line);
		free(file);
		exit(write(2, "Error\nCamera couldn't be set\n", 30));
	}
	if (is_set == 7)
		scene->init = 1;
	return (1);
}

int create_shape(char *slice, t_scene *scene, char * const line, char *file)
{
	static t_shape	*last_shape = NULL;

	if (*slice == 's' && !set_sphere(slice, scene, &last_shape))
	{
		free(line);
		free(file);
		exit(write(2, "Error\nSphere couldn't be set\n", 30));
	}
	else if (*slice == 'p' && !set_plane(slice, scene, &last_shape))
	{
		free(line);
		free(file);
		exit(write(2, "Error\nPlane couldn't be set\n", 29));
	}
	else if (*slice == 'c' && !set_cylinder(slice, scene, &last_shape))
	{
		free(line);
		free(file);
		exit(write(2, "Error\nCylinder couldn't be set\n", 32));
	}
	return (1);
}

void parse_line(char *const line, t_scene *scene, char *file)
{
	//WARN: do not modify, slice is used to not tamper with malloc-ed lines
	//		is_set is used as a check for multiple ambient and cameras 
	char		*slice;


	//printf("before if line is: %s\n", line);
	if (!*line)
		exit(write(2, "Error\nWrong format\n", 20));
	slice = skip_space(line);
	if (*slice && *(slice +  1) && is_space(*(slice + 1))
		&& (*slice == 'A' || *slice == 'L' || *slice == 'C'))
	{
		create_basics(slice, scene, line, file);
		//printf("slice is basic: %s", slice);
		//printf("line is basic: %s", line);
	}
	else if ((*slice && *(slice + 1)) && *(slice + 2)
			&& is_space (*(slice + 2)) && (ft_strncmp(slice, "sp", 2) == 0
			|| ft_strncmp(slice, "pl", 2) == 0
			|| ft_strncmp(slice, "cy", 2) == 0))
	{
		create_shape(slice, scene, line, file);
		//printf("slice is shape: %s", slice);
		//printf("line is shape: %s", line);
	}
	else
		//exit(write(2, "Error\nInvalid object\n", 22));
		exit(write(2, "Invalid shape\n", 14));
}

char	*get_file(int fd)
{
	char	*file;
	char	*str;

	str = get_next_line(fd);
	if (str == NULL)
		return (NULL);
	file = ft_strdup(str);
	while (str != NULL)
	{
		free(str);
		str = get_next_line(fd);
		//printf("str is: %s\n", str);
		if (str == NULL)
			break;
		file = ft_strappend(&file, str);
		//printf("file is: %s\n", file);
	}
	return (file);
}

char	*get_line(char **fslice)
{
	int		first;
	int		i;
	char	*head;

	first = 1;
	i = 0;
	head = *fslice;
	while (**fslice)
	{
		//printf("before if fslice is: %s\n", *fslice);
		if (is_break(*fslice))
		{
			//printf("enters if with fslice: %s\n", *fslice);
			if (!first)
				return(ft_substr(head, 0, i));
			else if (first)
				first = 0;
		}
		//printf("after if fslice is: %s\n", *fslice);
		(*fslice)++;
		i++;
	}
	return(ft_substr(head, 0, i));
}

void	vars_init(t_vars *vars)
{
	vars->mlx = mlx_init();
	if (vars->mlx == NULL)
		printf("MLX IS NULL");
	vars->win = mlx_new_window(vars->mlx, WIDTH, HEIGHT, "miniRT");
	if (vars->win == NULL)
		printf("WIN IS NULL");
	vars->img.img = mlx_new_image(vars->mlx, WIDTH, HEIGHT);
	if (vars->img.img == NULL)
		printf("IMG IS NULL");
	vars->img.addr = mlx_get_data_addr(vars->img.img, &vars->img.bpp,
										&vars->img.linelen, &vars->img.endian);
}

void minirt_init(t_vars *vars, int fd, t_scene *scene)
{
	char	*line;
	char	*file;
	char	*fslice;

	file = get_file(fd);
	if (!file)
		exit(write(2, "Error\nEmpty file\n", 18));
	//printf("FILE is: %s", file);
	ft_bzero(scene, sizeof(*scene));
	fslice = file;
	//printf("before while fslice is: %s\n", fslice);
	line = get_line(&fslice);
	//printf("before while line is: %s\n", line);
	while (*line && line != NULL)
	{
		//printf("enters and line is: %s\n", line);
		parse_line(line, scene, file);
		free(line);
		line = get_line(&fslice);
	}
	free(line);
	free(file);
	if (!scene->init)
		exit(write(2, "Error\nAmbient light, Camera or Light not set\n", 45));
	//printf("ENTERS PRINT\n");
	//printlights(scene);
	printshapes(scene);
	vars_init(vars);
}

t_vec3	get_xness(int numerator, int divisor, float half, t_vec3 vec)
{
	t_vec3		xness;
	float		scaling;

	scaling = 2.0f * ((float)numerator + 0.5f) / divisor - 1;

	xness.x = half * scaling * vec.x;
	xness.y = half * scaling * vec.y;
	xness.z = half * scaling * vec.z;

	//printf("xness with divisor %d is:\n", divisor);
	//printvec(xness);
	return (xness);
}

t_vec3	get_ray_vec(t_vec3 cam_dir, t_vec3 rightness, t_vec3 upness)
{
	t_vec3	vec;

	vec.x = cam_dir.x + rightness.x + upness.x;
	vec.y = cam_dir.y + rightness.y + upness.y;
	vec.z = cam_dir.z + rightness.z + upness.z;

	return (vec);
}

int	vectcmp(t_vec3 u, t_vec3 v)
{
	//printvec(u);
	//printvec(v);
	if (u.x != v.x)
		return (1);
	if (u.y != v.y)
		return (1);
	if (u.z != v.z)
		return (1);
	return (0);
}

char	same_plane(t_vec3 vec, t_vec3 right, t_vec3 up, t_vars *vars)
{
//	printf("WIDTH %% 2 is: %d\n", WIDTH % 2);
//	printf("WIDTH * 0.5 is: %.3f\n", (float)WIDTH * 0.5f);
//	printf("WIDTH / 2 is: %d\n", WIDTH / 2);
//	printf("HEIGHT %% 2 is: %d\n", HEIGHT % 2);
//	printf("HEIGHT * 0.5 is: %.3f\n", (float)HEIGHT * 0.5f);
//	printf("HEIGHT / 2 is: %d\n", HEIGHT / 2);
//	printf("vars->x_pix is: %d\n", vars->x_pix);
//	printf("vars->y_pix is: %d\n", vars->y_pix);
	(void)vars;
	if (dot_product(right, vec) == 0)
		return ('u');
	if (dot_product(up, vec) == 0)
		return ('r');
//	if (WIDTH % 2 == 0 && HEIGHT % 2 == 1 && vars->x_pix == WIDTH / 2 - 1)
//		return ('r');
//	if (HEIGHT % 2 == 0 && WIDTH % 2 == 1 && vars->y_pix == HEIGHT / 2 - 1)
//		return ('u');
	return ('\0');
}

void	put_pixel(t_image *img, int x, int y, int rgb)
{
	char	*dst;

	dst = img->addr + y * img->linelen + x * img->bpp / 8;
	*(unsigned int*)dst = rgb;
}

int	get_hzn_diff(char section, int x)
{
	int	hzn_diff;

	hzn_diff = x;
	if (section == 2 || section == 3)
		hzn_diff = WIDTH - x - 1;
	return (hzn_diff);
}

int	get_vert_diff(char section, int y)
{
	int	vert_diff;

	vert_diff = HEIGHT - y - 1;
	if (section == 1 || section == 3)
		vert_diff = y;
	return (vert_diff);
}

unsigned char	set_rgb(unsigned char c, int rgb)
{
	long	sum;

	sum = rgb + c;
	if (sum > 255)
		return (255);
	return (sum);
}

int	get_pixel_rgb(t_scene *scene, t_vars *vars, float angle)
{
	float			mult;
	int				rgb;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

	mult = scene->ambient.intensity;
	//printf("mult 1 is %.3f\n", mult);
	r = (unsigned char)((float)(vars->rcurrent->rgb >> 16 & 0xFF) * mult);
	g = (unsigned char)((float)(vars->rcurrent->rgb >> 8 & 0xFF) * mult);
	b = (unsigned char)((float)(vars->rcurrent->rgb & 0xFF) * mult);
	//if (t >= 0.001)
	//{
	//printf("---NEW---\n");
	//printf("pixel red value before: %d\n", r);
	//printf("pixel green value before: %d\n", g);
	//printf("pixel blue value before: %d\n",  b);
	//	return ((r << 16) | (g << 8) | b);
	//}
	mult = angle * scene->light.intensity;
	//printf("mult 2 is %.3f\n", mult);
	rgb = (unsigned char)((float)(vars->rcurrent->rgb >> 16 & 0xFF) * mult);
	r = set_rgb(r, rgb);
	rgb = (unsigned char)((float)(vars->rcurrent->rgb >> 8 & 0xFF) * mult);
	g = set_rgb(g, rgb);
	rgb = (unsigned char)((float)(vars->rcurrent->rgb & 0xFF) * mult);
	b = set_rgb(b, rgb);
	//printf("---NEW---\n");
	//printf("pixel red value before: %d\n", r);
	//printf("pixel green value before: %d\n", g);
	//printf("pixel blue value before: %d\n",  b);
	//printf("pixel red value after: %d\n", rgb >> 16 & 255);
	//printf("pixel green value after: %d\n", rgb >> 8 & 255);
	//printf("pixel blue value after: %d\n",  rgb & 255);
	return ((r << 16) | (g << 8) | b);
}

//float get_angle(t_vec3 u, t_vec3 v)
//{
//	float	dot;
//	float	ulen;
//	float	vlen;
//
//	//printf("first vec\n");
//	//printvec(u);
//	//printf("first vec\n");
//	//printvec(v);
//	dot = dot_product(u, v);
//	//printf("cos angle is: %.3f\n", dot);
//	//printf("angle is: %.3f\n", acosf(dot));
//	ulen = vec_length(u);
//	vlen = vec_length(v);
//
//	return ((dot / (ulen * vlen)));
//}

t_vec3	sphere_normal(t_ray ray, t_shape *sphere)
{
	t_vec3	normal;

	normal = sum_vec(ray.origin, scale_vec(ray.t, ray.dir));
	normal = sum_vec(normal, scale_vec(-1, sphere->pos));
	norm(&normal);
	return (normal);
}

t_vec3	plane_normal(t_ray ray, t_shape *plane)
{
	t_vec3	normal;

	/*printf("enters plane normal?\n");*/
	normal = plane->figure.plane.dir;
	/*printf("normal is before:\n");*/
	/*printvec(normal);*/
	if (dot_product(normal, ray.dir) < 0)
		return (normal);
	normal = scale_vec(-1, plane->figure.plane.dir);
	/*printf("normal is after:\n");*/
	//printvec(normal);
	return (normal);
}

t_vec3	get_normal(t_ray ray, t_vars *vars)
{

	//printf("enters normal with tag: %d\n", vars->rcurrent->shape_tag);
	//printf("rcurrent is: %p\n", vars->rcurrent);
	//printf("peta por rcurrent\n");
	if (vars->rcurrent->shape_tag == SPHERE_TAG)
		return (sphere_normal(ray, vars->rcurrent));
	else if (vars->rcurrent->shape_tag == PLANE_TAG)
		return (plane_normal(ray, vars->rcurrent));
	//else
	//	return (cylinder_normal());
	//printf("retorna?\n");
	return ((t_vec3){0,0,0});
}

float	get_angle(t_vec3 u, t_vec3 v)
{
	float	angle;
	float	cosang;

	cosang = dot_product(u, v);
	angle = acosf(cosang);
	if (angle < 0 || angle > M_PI_2)
		return (0);
	return (cosang);
}

void	get_pixel(t_ray r1, t_ray r2, t_vars *vars, t_scene *scene)
{
	float	angle;
	t_vec3	normal;
	int		rgb;
	int		vert_section;
	int		hzn_section;

//	printf("camera ray normalized dir is:\n");
//	printvec(r1.dir);
//	printf("light ray normalized dir is:\n");
//	printvec(r2.dir);
	//printf("entra\n");
	//printf("enters get_pixel with tag: %d\n", vars->rcurrent->shape_tag);
	normal = (t_vec3){0, 0, 0};
	angle = 0;
	hzn_section = get_hzn_diff(vars->section, vars->x_pix);
	vert_section = get_vert_diff(vars->section, vars->y_pix);
	//printf("pasa sections\n");
	//printf("pixel is: %d,%d\n", hzn_section, vert_section);
	//if (!vectcmp(sum_vec(r1.origin, scale_vec(r1.t, r1.dir)),
	//			sum_vec(r2.origin, scale_vec(r2.t, r2.dir))))
	//	return ;
	if (vars->rcurrent == vars->lcurrent)
	{
		normal = get_normal(r1, vars);
		//printf("pasa normal\n");
		angle = get_angle(scale_vec(-1, r2.dir), normal);
		//printf("pasa angle\n");
	}
	rgb = get_pixel_rgb(scene, vars, angle);
	if (HEIGHT < 200)
	{
		printf("hzn section is: %d\n", hzn_section);
		printf("vert section is: %d\n", vert_section);
		printf("pixel is: %d,%d\n", hzn_section, vert_section);
		printf("ray 1 has dir:\n");
		printvec(r1.dir);
		printf("ray 2 has dir:\n");
		printvec(r2.dir);
		printf("normal has dir:\n");
		printvec(normal);
		printf("cos angle is: %.3f\n", angle);
		printf("angle is: %.3f\n", 180 * acosf(angle) / M_PI);
		printf("pixel red value: %d\n", rgb >> 16 & 255);
		printf("pixel green value: %d\n", rgb >> 8 & 255);
		printf("pixel blue value: %d\n",  rgb & 255);
	}
	//printf("pasa rgb\n");
	put_pixel(&vars->img, hzn_section, vert_section, rgb);
	//mlx_put_image_to_window(vars->mlx, vars->win, vars->img.img, 0, 0);
}

float	get_least(float a, float b)
{
	if (!a && b)
		return (b);
	if (a && !b)
		return (a);
	if (a < b)
		return (a);
	return (b);
}

float	get_most(float a, float b)
{
	if (!a && b)
		return (b);
	if (a && !b)
		return (a);
	if (a > b)
		return (a);
	return (b);
}

//int bounce(t_ray ray, t_vars *vars, t_scene *scene)
//{
//	t_ray	bounced;
//	t_vec3	new_ray_dir;
//	if (ray.t == 0)
//		return (0);
//	bounced.origin = sum_vec(ray.origin, scale_vec(ray.t, ray.dir));
//	new_ray_dir = sum_vec(scene->light->pos, scale_vec(-1, bounced.origin));
//	norm(&new_ray_dir);
//	//get_pixel(ray.t, vars, scene);
//	return (1);
//}


//void	recursive_bounce(t_ray ray, t_vars *vars, t_scene *scene)
//{
//	int	retval;
//
//	// todo: finish bounce to light (also manage multiple rays, probably with a ray list)
	// note: there should be a reliable way to check if retval is equal to num_bounces
//	retval = bounce(prev, ray, vars, scene);
//	if (retval == 0)
//		return ;
//	if (retval == num_bounces)
//		return (get_pixel(ray.t, vars, scene));
//	recursive_bounce(prev, ray, vars, scene);
//}
float	check_intersect(t_ray ray, t_shape *head)
{
	// TODO: finish plane and cylinder intersects
	if (HEIGHT < 200)
		printf("enters check intersect\n");
	if (head->shape_tag == SPHERE_TAG)
		return (intersect_sphere(ray, head->pos, 0.5f * head->figure.sphere.diameter));
	else if (head->shape_tag == PLANE_TAG)
		return (intersect_plane(ray, head->pos, head->figure.plane.dir));
	return (0);
	//else
	//	t = intersect_cylinder(ray, head->pos, head->figure.cylinder);
	//		printf ("t is: %.7f\n", t);
		//if (head->shape_tag == PLANE_TAG)
			//t = intersect_plane(ray, head->pos, head->figure.plane.dir / 2);
	//		printf ("t is: %.7f\n", t);
	//printf("current flips\n");
	//printf("current red value: %d\n", ((*current)->rgb >> 16) & 255);
	//printf("current green value: %d\n", ((*current)->rgb >> 8) & 255);
	//printf("current blue value: %d\n",  ((*current)->rgb) & 255);
	//if (ray->t != 0)
	//{
	//}
}

float cast_ray(t_ray ray, t_shape **current, t_scene *scene)
{
	//printf("ray is cast:\n");
	//printvec(ray.dir);
	float	temp;
	t_shape *head;

	head = scene->headshape;
	ray.t = 0;
	temp = 0;
	//printf("NEW LOOP\n");
	while (head != NULL)
	{
		if (HEIGHT < 200)
		{
			printf("enters cast_ray loop\n");
			printf("head is: %p\n", head);
			printf("ray.t is: %.3f\n", ray.t);
			printf("temp is: %.3f\n", temp);
		}
		ray.t = check_intersect(ray, head);
		if (HEIGHT < 200)
			printf("t after intersect is: %.9f\n", ray.t);
		if (temp == 0 && ray.t != 0)
			*current = head;
		else if (temp != 0 && ray.t != 0 && ray.t < temp)
			*current = head;
		temp = get_least(temp, ray.t);
		head = head->next;
	}
	if (HEIGHT < 200)
			printf("exits cast_ray loop\n");
	return (get_least(ray.t, temp));
}

//float	cast_light_ray(t_ray ray, t_scene *scene, float max)
//{
//	//printf("ray is cast:\n");
//	//printvec(ray.dir);
//	t_shape *head;
//	float	temp;
//
//	head = scene->headshape;
//	ray.t = 0;
//	temp = 0;
//	while (head != NULL)
//	{
//		//printf("enters cast_ray loop\n");
//		check_light_intersect(&ray, head);
//		temp = get_most(ray.t, temp, max);
//		head = head->next;
//	}
//	return (get_most(ray.t, temp, max));
//}
//
t_ray	light_bounce(t_ray ray, t_vars *vars, t_scene *scene)
{
	t_ray light_ray;
	//float distance;

	//printf("ray enters light_bounce with:\n");
	//printvec(ray.dir);
	light_ray.origin = scene->light.pos;
	light_ray.dir = sum_vec(sum_vec(ray.origin, scale_vec(ray.t, ray.dir)), scale_vec(-1, light_ray.origin));
	//printf("light ray dir is:\n");
	//printvec(light_ray.dir);
	//distance = vec_length(light_ray.dir);
	//printf("light ray distance is: %.3f\n", distance);
	norm(&light_ray.dir);
	//printf("light ray origin is:\n");
	//printvec(light_ray.origin);
	//printf("light ray normalized dir is:\n");
	//printvec(light_ray.dir);
	//printf("LIGHT RAY ENTERS CAST RAY\n");
	light_ray.t = cast_ray(light_ray, &vars->lcurrent, scene);
	//printf ("t base ray is: %.7f\n", ray.t);
	//printf ("t light ray is: %.7f\n", light_ray.t);
	return (light_ray);
}

//int	pcmp(t_vec3 p1, t_vec3 p2)
//{
//	if (fabsf(p1.x) > fabsf(p2.x) - DELIM && fabsf(p1.x) < fabsf(p2.x) + DELIM)
//		return (1);
//	if (fabsf(p1.y) > fabsf(p2.y) - DELIM && fabsf(p1.y) < fabsf(p2.y) + DELIM)
//		return (1);
//	if (fabsf(p1.z) > fabsf(p2.z) - DELIM && fabsf(p1.z) < fabsf(p2.z) + DELIM)
//		return (1);
//	return (0);
//}

void	trace(t_ray ray, t_vars *vars, t_scene *scene)
{
	t_ray light_ray;

	//printf("enters trace\n");
	//i = -1;
	// INFO gets the first intersection, if 0, return
	//while (++i < NUM_BOUNCES)
	//{
	// INFO gets the bounces, if any bounce intersects while going to the light, return
	vars->rcurrent = NULL;
	vars->lcurrent = NULL;
	ray.t = cast_ray(ray, &vars->rcurrent, scene);
	//printf ("t is: %.7f\n", ray.t);
	if (vars->rcurrent == NULL)
		return ;
	light_ray = make_ray(make_vec(0,0,0), make_vec(1,1,0));
	if (ray.t != 0)
		light_ray = light_bounce(ray, vars, scene);
	//printf("ray current is: %p\n", vars->rcurrent);
	//printf("ray current tag is: %d\n", vars->rcurrent->shape_tag);
	//printf("light current is: %p\n", vars->lcurrent);
	//printf("light current tag is: %d\n", vars->lcurrent->shape_tag);
	/*if (vars->rcurrent != vars->lcurrent)*/
	/*	return (put_dark_pixel(vars, scene));*/
	get_pixel(ray, light_ray, vars, scene);
	//printf("exits light_bounce\n");
	// INFO sums the values of the bounces, divides by the number of bounces until finding a 0
	//}
}


//t_vec3	rot(t_vec3 w, t_vec3 u, t_vec3 v, float angle)
//{
//	t_vec3	perpendicular;
//	t_vec3	rotated;
//	t_vec3	horizontal;
//	t_vec3	vertical;
//
//	//printf("rotated by: %.3f\n", angle * 180 / M_PI);
//	//printf("sin is: %.3f\n", sin(angle));
//	//printf("cos is: %.3f\n", cos(angle));
//	horizontal = scale_vec(sinf(angle), u);
//	vertical = scale_vec(cosf(angle), v);
//	perpendicular = sum_vec(horizontal, vertical);
//	//printf("paralel is:\n");
//	//printvec(w);
//	//printf("perpendicular is:\n");
//	//printvec(perpendicular);
//	rotated = sum_vec(w, perpendicular);
//	//printf("rotated is:\n");
//	//printvec(rotated);
//	return (rotated);
//}
//
//void	cast_multiple_rays(t_ray ray, t_vars *vars, t_scene *scene)
//{
//	char	plane;
//	t_vec3	w;
//	t_vec3	u;
//	t_vec3	v;
//	float	scale_factor;
//
//	// NOTE: rotations here are counterclockwise
//
//	//printf("new base ray\n");
//	if (!vectcmp(scene->camera.forward, ray.dir))
//		return (cast_ray(ray, vars, scene));
//	scale_factor = dot_product(scene->camera.forward, ray.dir);
//	u = cross_product(scene->camera.forward, ray.dir);
//	norm(&u);
//	//printf("scale factor is: %.3f\n", scale_factor);
//	//printf("u is:\n");
//	//printvec(u);
//	v = cross_product(u, scene->camera.forward);
//	norm(&v);
//	//printf("v is:\n");
//	//printvec(v);
//	u = scale_vec(sqrtf(1 - scale_factor * scale_factor), u);
//	//printf("u after scaling is:\n");
//	//printvec(u);
//	v = scale_vec(sqrtf(1 - scale_factor * scale_factor), v);
//	//printf("v after scaling is:\n");
//	//printvec(v);
//	w = scale_vec(scale_factor, scene->camera.forward);
//	plane = same_plane(ray.dir, scene->camera.right, scene->camera.up);
//	if (plane == 'r' || plane == 'u')
//		return (cast_ray(ray, vars, scene),
//		cast_ray(make_ray(ray.origin, rot(w, u, v, M_PI), vars, M_PI), vars, scene));
//	return (cast_ray(ray, vars, scene),
//			cast_ray(make_ray(ray.origin, rot(w, u, v, M_PI_2), vars, M_PI_2), vars, scene),
//			cast_ray(make_ray(ray.origin, rot(w, u, v, M_PI), vars, M_PI), vars, scene),
//			cast_ray(make_ray(ray.origin, rot(w, u, v, -M_PI_2), vars, -M_PI_2), vars, scene));
//}

float	pyt(float a, float b, float c)
{
	return (sqrtf(a * a + b * b + c * c));
}

t_vec3	flip_hzn(t_vec3 proj, t_vars *vars, t_scene *scene)
{
	t_vec3	flipped;
	t_vec3	right;
	t_vec3	up;
	t_vec3	forward;

	right = scale_vec(-proj.x, scene->camera.right);
	up = scale_vec(proj.y, scene->camera.up);
	forward = scale_vec(proj.z, scene->camera.forward);
	flipped = sum_vec(sum_vec(right, up), forward);
	//x = pyt(scene->camera.right.x, scene->camera.up.x, scene->camera.forward.x);
	//y = pyt(scene->camera.right.y, scene->camera.up.y, scene->camera.forward.y);
	//z = pyt(scene->camera.right.z, scene->camera.up.z, scene->camera.forward.z);
	//flipped.x = -proj.x * x;
	//flipped.y = proj.y * y;
	//flipped.z = proj.z * z;
	vars->section = HZN;
	//printf("flipped horizontal is:\n");
	//printvec(flipped);
	return (flipped);
}

t_vec3	flip_vert(t_vec3 proj, t_vars *vars, t_scene *scene)
{
	t_vec3	flipped;
	//float	x;
	//float	y;
	//float	z;
	t_vec3	right;
	t_vec3	up;
	t_vec3	forward;

	right = scale_vec(proj.x, scene->camera.right);
	up = scale_vec(-proj.y, scene->camera.up);
	forward = scale_vec(proj.z, scene->camera.forward);
	flipped = sum_vec(sum_vec(right, up), forward);
	//x = pyt(scene->camera.right.x, scene->camera.up.x, scene->camera.forward.x);
	//y = pyt(scene->camera.right.y, scene->camera.up.y, scene->camera.forward.y);
	//z = pyt(scene->camera.right.z, scene->camera.up.z, scene->camera.forward.z);
	//flipped.x = proj.x * x;
	//flipped.y = -proj.y * y;
	//flipped.z = proj.z * z;
	vars->section = VERT;
	//printf("flipped vertical is:\n");
	//printvec(flipped);
	return (flipped);
}

t_vec3	flip_both(t_vec3 proj, t_vars *vars, t_scene *scene)
{
	t_vec3	flipped;
	//float	x;
	//float	y;
	//float	z;
	t_vec3	right;
	t_vec3	up;
	t_vec3	forward;

	right = scale_vec(-proj.x, scene->camera.right);
	up = scale_vec(-proj.y, scene->camera.up);
	forward = scale_vec(proj.z, scene->camera.forward);
	flipped = sum_vec(sum_vec(right, up), forward);
	//x = pyt(scene->camera.right.x, scene->camera.up.x, scene->camera.forward.x);
	//y = pyt(scene->camera.right.y, scene->camera.up.y, scene->camera.forward.y);
	//z = pyt(scene->camera.right.z, scene->camera.up.z, scene->camera.forward.z);
	//flipped.x = -proj.x * x;
	//flipped.y = -proj.y * y;
	//flipped.z = proj.z * z;
	vars->section = BOTH;
	//printf("flipped both is:\n");
	//printvec(flipped);
	return (flipped);
}

void	cast_multiple_rays(t_ray ray, t_vars *vars, t_scene *scene)
{
	char	plane;
	t_vec3	proj;

	//printf("enters cast_multiple_rays with ray:\n");
	//printvec(ray.dir);
	if (!vectcmp(scene->camera.forward, ray.dir))
		return (trace(ray, vars, scene));
	proj.x = dot_product(scene->camera.right, ray.dir);
	proj.y = dot_product(scene->camera.up, ray.dir);
	proj.z = sqrtf(1 - proj.x * proj.x - proj.y * proj.y);
	//printf("projection is:\n");
	//printvec(proj);
	vars->section = BASE;
	plane = same_plane(ray.dir, scene->camera.right, scene->camera.up, vars);
	if (plane == 'r')
		return (trace(ray, vars, scene),
				trace(make_ray(ray.origin, flip_hzn(proj, vars, scene)),
						vars, scene));
	else if (plane == 'u')
		return (trace(ray, vars, scene),
				trace(make_ray(ray.origin, flip_vert(proj, vars, scene)),
						vars, scene));
	//printf("returns multiple rays\n");
	return (trace(ray, vars, scene),
	trace(make_ray(ray.origin, flip_hzn(proj, vars, scene)), vars, scene),
	trace(make_ray(ray.origin, flip_both(proj, vars, scene)), vars, scene),
	trace(make_ray(ray.origin, flip_vert(proj, vars, scene)), vars, scene));
}

float	set_fov(int fov)
{
	if (fov == 180)
		return (tanf(M_PI * 89.75 / 180));
	return (tanf(0.5f * M_PI * fov / 180));
}

void	raytrace(t_vars *vars, t_scene *scene)
{
	float	half_width;
	float	half_height;
	t_ray	ray;

	printf("\n----RAYTRACE START----\n");
	vars->x_pix = -1;
	vars->y_pix = -1;
	half_width = set_fov(scene->camera.fov);
	half_height = half_width * WIDTH / HEIGHT;
//	printf("height: %d\nwidth:%d\n", HEIGHT, WIDTH);
//	width = get_screen_width();
//	height = get_screen_height();
	while (++vars->y_pix < HEIGHT / 2 + HEIGHT % 2)
	{
		while (++vars->x_pix < WIDTH / 2 + WIDTH % 2)
		{
			if (HEIGHT < 200)
				printf("pixel should be: %d,%d\n", vars->x_pix, vars->y_pix);
			ray.origin = scene->camera.pos;
			ray.dir = get_ray_vec(scene->camera.forward,
						get_xness(vars->x_pix, WIDTH, half_width, scene->camera.right),
						get_xness(vars->y_pix, HEIGHT, half_height, scene->camera.up));
			norm(&ray.dir);
			cast_multiple_rays(ray, vars, scene);
	//		vars->x_pix += 3;
		}
		vars->x_pix = -1;
	//	vars->y_pix += 3;
	}
	printf("\n----RAYTRACE END----\n");
}

int	check_extension(char *name)
{
	int	len;

	len = 0;
	if (!name)
		return (0);
	while (name[len])
		len++;
	if (len >= 3 && !strncmp(&name[len - 3],".rt", 3))
		return (1);
	return (0);
}

int	main(int argc, char **argv)
{
	t_vars	vars;
	int		fd;
	t_scene	scene;
	
	if (argc != 2)
		return (write(2, "Error\nIncorrect number of arguments\n", 37));
	if (!check_extension(argv[1]))
		return (write(2, "Error\nInvalid extension\n", 25));
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return (write(2, "Error\nFile not found\n", 22));
	minirt_init(&vars, fd, &scene);
	raytrace(&vars, &scene);
	key_hooks(&vars);
	mlx_hook(vars.win, DestroyNotify, 0, close_win, &vars);
	mlx_put_image_to_window(vars.mlx, vars.win, vars.img.img, 0, 0);
	mlx_loop(vars.mlx);
	return (0);
}
