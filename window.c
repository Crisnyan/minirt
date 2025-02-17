//#define HEIGHT 1080
//#define WIDTH 1920
//#define HEIGHT 720
//#define WIDTH 1280
//#define HEIGHT 101
//#define WIDTH 101
#define HEIGHT 301
#define WIDTH 301

#define SPHERE_TAG 0
#define PLANE_TAG 1
#define CYLINDER_TAG 2

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

typedef struct s_vars
{
	void		*mlx;
	t_win_list	*win;
	int			x_pix;
	int			y_pix;
}	t_vars;

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
	int		trgb;
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
	struct s_light	*next;
	float			intensity;
	int				trgb;
}	t_light;

typedef struct s_shape
{
	//TODO:	figure should contain a funtion that describes the surface of the figure, i.e. x^2+y^2=r^2
	t_figure		figure;	
	t_vec3			pos;
	struct s_shape	*next;
	int				trgb;
	char			shape_tag;
}	t_shape;

typedef struct s_scene
{
	t_amblight	ambient;
	t_camera	camera;
	t_light		*headlight;
	t_shape		*headshape;
}	t_scene;

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
	printf("x: %.3f\n", vec.x);
	printf("y: %.3f\n", vec.y);
	printf("z: %.3f\n", vec.z);
}

void	printlights(t_scene *scene)
{
	int	i;

	i = 1;
	while (scene->headlight)
	{
		printf("light %d\n", i);
		printf("light pos.x: %.1f\n", scene->headlight->pos.x);
		printf("light pos.y: %.1f\n", scene->headlight->pos.y);
		printf("light pos.z: %.1f\n", scene->headlight->pos.z);
		printf("light intensity: %.1f\n", scene->headlight->intensity);
		printf("light red value: %d\n", (scene->headlight->trgb >> 16) & 255);
		printf("light green value: %d\n", (scene->headlight->trgb >> 8) & 255);
		printf("light blue value: %d\n",  (scene->headlight->trgb) & 255);
		scene->headlight = scene->headlight->next;
		i++;
	}
	
}

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
		printf("shape red value: %d\n", (ptr->trgb >> 16) & 255);
		printf("shape green value: %d\n", (ptr->trgb >> 8) & 255);
		printf("shape blue value: %d\n",  (ptr->trgb) & 255);
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

char	*bitoi(char **str, int *trgb, char c)
{
	int	num;

	//printf("entra en bitoi\n");
	if (get_byte(str, &num) == NULL)
		return (NULL);
	if (c == 't')
		*trgb |= (num << 24);
	else if (c == 'r')
		*trgb |= (num << 16);
	else if (c == 'g')
		*trgb |= (num << 8);
	else if (c == 'b')
		*trgb |= num;
	//printf("sale de bitoi\n");
	return (*str);
}

t_vec3	quadratic(float a, float b, float c)
{
	t_vec3 value;

	value.x = b * b - 4 * a * c;
	if (value.x < 0)
	{
		printf("has no roots\n");
		value.x = 0;
		return (value);
	}
	value.y = (-b + sqrtf(value.x)) / (2 * a);
	if (value.x == 0)
	{
		printf("has only one root\n");
		//printf("root is: %.7f\n", value.y);
		value.x = 1;
		return (value);
	}
	else 
	{
		value.z = (-b - sqrtf(value.x)) / (2 * a);
		printf("has two roots\n");
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
	//printf("red value is: %d\n", *rgb >> 16);
	//printf("green value is: %d\n", (*rgb >> 8) & 255);
	//printf("blue value is: %d\n", *rgb & 255);
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
	if (*fov < 0 || *fov > 180)
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

int	create_lnode(t_light **last_light)
{ 
	//printf("enters create_node with last_light: %p\n", *last_light);
	*last_light = malloc(sizeof(t_light));
	//printf("create_node makes last_light: %p\n", *last_light);
	if (!(*last_light))
		return (0);
	(*last_light)->trgb = (255 << 16) | (255 << 8) | 255;
	(*last_light)->next = NULL;
	return (1);
}

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
	return ((t_vec3){s * v.x, s * v.y, s * v.z});
}

t_vec3	sum_vec(t_vec3 u, t_vec3 v)
{
	t_vec3	vec;

	vec.x = u.x + v.x;
	vec.y = u.y + v.y;
	vec.z = u.z + v.z;
	return (vec);
}

t_vec3	cross_product(t_vec3 u, t_vec3 v)
{
	t_vec3	vec;

	vec.x = u.y * v.z - u.z * v.y;
	vec.y = u.z * v.x - u.x * v.z;
	vec.z = u.x * v.y - u.y * v.x;
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

t_ray	make_ray(t_vec3 o, t_vec3 d, t_vars *vars, float angle)
{
	t_ray	ray;

	ray.origin = o;
	ray.dir = d;
	//if (angle == M_PI_2)
	(void)vars;
	(void)angle;
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

	vec = check_unidim_r(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	vec = check_bidim_r(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	vec.y = 0;
	if ((cam_dir.x > 0 && cam_dir.z > 0) || (cam_dir.x < 0 && cam_dir.z > 0))
	{
		vec.x = cam_dir.z;
		vec.z = -cam_dir.x;
	}
	else
	{
		vec.x = -cam_dir.z;
		vec.z = cam_dir.x;
	}
	return (vec);
}

t_vec3	get_up(t_vec3 cam_dir, t_vec3 right)
{
	t_vec3	vec;

	vec = check_unidim_u(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	vec = check_bidim_u(cam_dir);
	if (!is_null_vec(vec))
		return (vec);
	vec = cross_product(right, cam_dir);
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
	if ((*is_set & 1) == 1)
		 return (0);
	//printf("enters get_intensity\n");
	slice = get_intensity(slice, &scene->ambient.intensity);
	//printf("exits get_intensity\n");
	if (slice == NULL)
		return (0);
	//printf("enters get_rgb\n");
	//printf("slice is: %s", slice);
	slice = get_rgb(slice, &scene->ambient.trgb);
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
	printf("amblight red value: %d\n", (scene->ambient.trgb >> 16) & 255);
	printf("amblight green value: %d\n", (scene->ambient.trgb >> 8) & 255);
	printf("amblight blue value: %d\n",scene->ambient.trgb & 255);
	//printf("exits set_amblight\n");
	return (1);
}

int	set_light(char *slice, t_scene *scene)
{
	static t_light	*last_light = NULL;
	
	//printf("\nenters with headlight: %p\n\n", scene->headlight);
	//printf("enters set_light\n");
	if (last_light != NULL)
	{
		if (!create_lnode(&last_light->next))
			return (0);
		last_light = last_light->next;
	}
	else 
	{
		if (!create_lnode(&scene->headlight))
			return (0);
		last_light = scene->headlight;
	}
	slice = get_vec(slice, &last_light->pos);
	if (slice == NULL)
		return (0);
	slice = get_intensity(slice, &last_light->intensity);
	if (slice == NULL)
		return (0);
	printf("slice is: %s", slice);
	while (*slice)
	{
		if (!is_space(*slice))
		{
			slice = get_rgb(slice, &last_light->trgb);
			//printf("slice is: %s", slice);
			break;
		}
		slice++;
	}
	if (slice == NULL)
		return (0);
	//printf("enters if with last_light: %p\n", last_light);
	//printf("exits if with last_light: %p\n", last_light);
	//printf("\nexits with headlight: %p\n\n", scene->headlight);
	return (1);
}

int	set_camera(char *slice, t_scene *scene, char *is_set)
{
	//printf("enters set_camera\n");
	if (*is_set >> 1 == 1)
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
	//*is_set += 2;
	scene->camera.right = get_right(scene->camera.forward);
	scene->camera.up = get_up(scene->camera.forward, scene->camera.right);
	printf("camera pos.x: %.1f\n", scene->camera.pos.x);
	printf("camera pos.y: %.1f\n", scene->camera.pos.y);
	printf("camera pos.z: %.1f\n", scene->camera.pos.z);
	printf("camera forward.x: %.1f\n", scene->camera.forward.x);
	printf("camera forward.y: %.1f\n", scene->camera.forward.y);
	printf("camera forward.z: %.1f\n", scene->camera.forward.z);
	printf("camera right.x: %.1f\n", scene->camera.right.x);
	printf("camera right.y: %.1f\n", scene->camera.right.y);
	printf("camera right.z: %.1f\n", scene->camera.right.z);
	printf("camera up.x: %.1f\n", scene->camera.up.x);
	printf("camera up.y: %.1f\n", scene->camera.up.y);
	printf("camera up.z: %.1f\n", scene->camera.up.z);
	printf("camera fov: %d\n", scene->camera.fov);
	//printf("exits set_camera\n");
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
	//printf("enters intersect sphere\n");
	d.x = r.origin.x - o.x;
	d.y = r.origin.y - o.y;
	d.z = r.origin.z - o.z;
	abc.x = r.dir.x * r.dir.x + r.dir.y * r.dir.y + r.dir.z * r.dir.z;
	abc.y = 2 * (r.dir.x * d.x + r.dir.y * d.y + r.dir.z * d.z);
	abc.z = d.x * d.x + d.y * d.y + d.z * d.z - radius * radius;
	//printf("a is: %.3f\n", abc.x);
	//printf("b is: %.3f\n", abc.y);
	//printf("c is: %.3f\n", abc.z);
	q_vals = quadratic(abc.x, abc.y, abc.z);
	if (q_vals.x == 0)
		return (0);
	else if (q_vals.x == 1)
		return (q_vals.y);
	if (fabsf(q_vals.y) < fabsf(q_vals.z))
		return (fabsf(q_vals.y));
	return (fabsf(q_vals.z));
}

int	set_sphere(char *slice, t_scene *scene, t_shape **last_shape)
{
	//printf("enters set_sphere\n");
	//printf("\nenters with headlight: %p\n\n", scene->headlight);
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
	slice = get_rgb(slice, &(*last_shape)->trgb);
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
	slice = get_rgb(slice, &(*last_shape)->trgb);
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
	slice = get_rgb(slice, &(*last_shape)->trgb);
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
	else if (*slice == 'L' && !set_light(++slice, scene))
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
	//printf("ENTERS PRINT\n");
	printlights(scene);
	printshapes(scene);
	vars->mlx = mlx_init();
	vars->win = mlx_new_window(vars->mlx, WIDTH, HEIGHT, "miniRT");
}

t_vec3	get_xness(int numerator, int divisor, float half, t_vec3 vec)
{
	t_vec3		xness;
	float		scaling;

	scaling = 2.0f * ((float)numerator + 0.5f) / divisor - 1;

	xness.x = half * scaling * vec.x;
	xness.y = half * scaling * vec.y;
	xness.z = half * scaling * vec.z;

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
	if (u.x != v.x)
		return (1);
	if (u.y != v.y)
		return (1);
	if (u.z != v.z)
		return (1);
	return (0);
}

char	same_plane(t_vec3 vec, t_vec3 right, t_vec3 up)
{
	if (dot_product(right, vec) == 0)
		return ('u');
	if (dot_product(up, vec) == 0)
		return ('r');
	return ('\0');
}

void	ray_cast(t_ray ray, t_vars *vars, t_scene *scene)
{
	printf("ray is cast with direction.x: %.3f\n", ray.dir.x);
	printf("ray is cast with direction.y: %.3f\n", ray.dir.y);
	printf("ray is cast with direction.z: %.3f\n", ray.dir.z);
	t_shape *head;
	float	t;

	// TODO: Finsh plane and cylinder intersections
	head = scene->headshape;
	t = 0;
	while (head != NULL)
	{
		if (head->shape_tag == SPHERE_TAG)
		{
			t = intersect_sphere(ray, head->pos, head->figure.sphere.diameter / 2);
			printf ("t is: %.7f\n", t);
		}
		//else if (head->shape_tag == PLANE_TAG);
		//else ;
		head = head->next;
	}
	(void)vars;
	return ;
}

t_vec3	rot(t_vec3 w, t_vec3 u, t_vec3 v, float angle)
{
	t_vec3	perpendicular;
	t_vec3	rotated;
	t_vec3	horizontal;
	t_vec3	vertical;

	//printf("rotated by: %.3f\n", angle * 180 / M_PI);
	//printf("sin is: %.3f\n", sin(angle));
	//printf("cos is: %.3f\n", cos(angle));
	horizontal = scale_vec(sinf(angle), u);
	vertical = scale_vec(cosf(angle), v);
	perpendicular = sum_vec(horizontal, vertical);
	//printf("paralel is:\n");
	//printvec(w);
	//printf("perpendicular is:\n");
	//printvec(perpendicular);
	rotated = sum_vec(w, perpendicular);
	//printf("rotated is:\n");
	//printvec(rotated);
	return (rotated);
}

void	cast_multiple_rays(t_ray ray, t_vars *vars, t_scene *scene)
{
	char	plane;
	t_vec3	w;
	t_vec3	u;
	t_vec3	v;
	float	scale_factor;

	// NOTE: rotations here are counterclockwise
	//printf("new base ray\n");
	if (!vectcmp(scene->camera.forward, ray.dir))
		return (ray_cast(ray, vars, scene));
	scale_factor = dot_product(scene->camera.forward, ray.dir);
	u = cross_product(scene->camera.forward, ray.dir);
	norm(&u);
	//printf("scale factor is: %.3f\n", scale_factor);
	//printf("u is:\n");
	//printvec(u);
	v = cross_product(u, scene->camera.forward);
	norm(&v);
	//printf("v is:\n");
	//printvec(v);
	u = scale_vec(sqrt(1 - scale_factor * scale_factor), u);
	//printf("u after scaling is:\n");
	//printvec(u);
	v = scale_vec(sqrt(1 - scale_factor * scale_factor), v);
	//printf("v after scaling is:\n");
	//printvec(v);
	w = scale_vec(scale_factor, scene->camera.forward);
	plane = same_plane(ray.dir, scene->camera.right, scene->camera.up);
	if (plane == 'r' || plane == 'u')
		return (ray_cast(ray, vars, scene),
		ray_cast(make_ray(ray.origin, rot(w, u, v, M_PI), vars, M_PI), vars, scene));
	return (ray_cast(ray, vars, scene),
			ray_cast(make_ray(ray.origin, rot(w, u, v, M_PI_2), vars, M_PI_2), vars, scene),
			ray_cast(make_ray(ray.origin, rot(w, u, v, M_PI), vars, M_PI), vars, scene),
			ray_cast(make_ray(ray.origin, rot(w, u, v, -M_PI_2), vars, -M_PI_2), vars, scene));
}

void	raytrace(t_vars *vars, t_scene *scene)
{
	float	half_width;
	float	half_height;
	t_ray	ray;

	vars->x_pix = -1;
	vars->y_pix = -1;
	half_width = tanf(0.5f * scene->camera.fov);
	half_height = half_width * WIDTH / HEIGHT;
	while (++vars->y_pix < HEIGHT / 2 + HEIGHT % 2)
	{
		while (++vars->x_pix < WIDTH / 2 + WIDTH % 2)
		{
			ray.origin = scene->camera.pos;
			ray.dir = get_ray_vec(scene->camera.forward,
						get_xness(vars->y_pix, WIDTH, half_width, scene->camera.right),
						get_xness(vars->x_pix, HEIGHT, half_height, scene->camera.up));
			norm(&ray.dir);
			cast_multiple_rays(ray, vars, scene);
		}
		vars->x_pix = -1;
	}
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
	mlx_loop(vars.mlx);
	return (0);
}
