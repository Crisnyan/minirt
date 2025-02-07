#define G_HEIGHT 1080
#define G_WIDTH 1920
#define HEIGHT 720
#define WIDTH 1280

#define ESC 65307
#define W_KEY 119
#define A_KEY 97
#define S_KEY 115
#define D_KEY 100

#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include "mlx/mlx.h"
#include "mlx/mlx_int.h"
#include "libft/libft.h"

typedef struct s_vars
{
	void		*mlx;
	t_win_list	*win;
}	t_vars;

typedef	struct s_vec3
{
	float	x;
	float	y;
	float	z;
}	t_vec3;

typedef struct	s_sphere
{
	float	diameter;	
	float	(*create_sphere)(t_vec3, float);
}	t_sphere;

typedef struct	s_plane
{
	t_vec3	dir;
	float 	(*create_plane)(t_vec3, t_vec3);
}	t_plane;

typedef struct	s_cylinder
{
	t_vec3	dir;
	float	diameter;	
	float	height;	
	float	(*create_cylinder)(t_vec3, float, float);
}	t_cylinder;

typedef union figure
{
	t_sphere	sphere;
	t_plane		plane;
	t_cylinder	cylinder;
}	u_figure;

typedef struct s_amblight
{
	float	intensity;
	int		trgb;
} t_amblight;

typedef struct s_camera
{
	t_vec3	pos;
	t_vec3	dir;
	int		fov;
}	t_camera;

typedef struct s_light
{
	int				trgb;
	t_vec3			pos;
	float			intensity;
	struct s_light	*next;
}	t_light;

typedef struct s_shape
{
	//TODO:	figure should contain a funtion that describes the surface of the figure, i.e. x^2+y^2=r^2
	t_vec3			pos;
	u_figure		figure;	
	struct s_shape	*next;
}	t_shape;

typedef struct s_scene
{
	t_amblight	ambient;
	t_camera	camera;
	t_light		*lights;
	t_shape		*shapes;
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

char	*skip_space(char *const line)
{
	char *slice;

	slice = line;
	while (*slice && is_space(*slice))
		 slice++;
	return (slice);
}

char *get_whole(char **str, int *whole, int *neg)
{
	int res;
	char *first;

	res = 0;
	first = *str;
	while (**str && **str != '.')
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

char *get_decimal(char **str, int *decimal, int *counter)
{
	*decimal = 0;
	*counter = 0;
	if (**str == '.')
		(*str)++;
	while (**str && !is_space(**str))
	{
		printf("get_decimal in\n");
		if (!is_digit(**str))
			return (NULL);
		*decimal = *decimal * 10 + (**str - '0');
		printf("decimal is: %d\n", *decimal);
		(*counter)++;
		(*str)++;
	}
	printf("get_decimal out\n");
	printf("counter is: %d\n", *decimal);
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
	int whole;
	int decimal;
	int counter;
	int neg;
	
	counter = 0;
	neg = 1;
	if (get_whole(str, &whole, &neg) == NULL)
		return (NULL);
	printf("atof whole side is: %d\n", whole);
	if (get_decimal(str, &decimal, &counter) ==  NULL)
		return (NULL);
	printf("atof decimal side is: %d\n", decimal);
	*num = neg * ((float)whole + (float)decimal * pow_counter(counter));
	printf("atof final number is: %.2f\n", *num);

	return (*str);
}

char	*get_byte(char **str, int *num)
{
	printf("entra en get_byte\n");
	*num = 0;
	while (**str && **str != ',' && **str != '\n')
	{
	printf("get_byte in\n");
		if (!is_digit(**str))
			return (NULL);
		*num = *num * 10 + (**str - '0');
		printf("get_byte num is: %d\n", *num);
		(*str)++;
	}
	printf("get_byte out\n");
	if (*num > 255)
		return (NULL);
	printf("sale de get_byte\n");
	return (*str);
}

char	*bitoi(char **str, int *trgb, char c)
{
	int	num;

	printf("entra en bitoi\n");
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
	printf("sale de bitoi\n");
	return (*str);
}

char	*get_pos(char *slice, t_vec3 *pos)
{
	slice = skip_space(slice);
	if (ft_atof(&slice, &pos->x) == NULL)
		return (NULL);
	if (*slice && (*slice)++ != ',')
		return (NULL);
	if (ft_atof(&slice, &pos->y) == NULL)
		return (NULL);
	if (*slice && (*slice)++ != ',')
		return (NULL);
	if (ft_atof(&slice, &pos->z) == NULL)
		return (NULL);
	return (slice);
}

char	*get_dir(char *slice, t_vec3 *dir)
{
	slice = skip_space(slice);
	if (ft_atof(&slice, &dir->x) == NULL)
		return (NULL);
	if (*slice && *(slice++) != ',')
		return (NULL);
	if (ft_atof(&slice, &dir->y) == NULL)
		return (NULL);
	if (*slice && *(slice++) != ',')
		return (NULL);
	if (ft_atof(&slice, &dir->z) == NULL)
		return (NULL);
	return (slice);
}

char	*get_rgb(char *slice, int *rgb)
{
	slice = skip_space(slice);
	*rgb = 0;
	printf("slice before first bitoi: %s", slice);
	if (bitoi(&slice, rgb, 'r') == NULL)
		return (NULL);
	printf("slice after first bitoi: %s", slice);
	if (*slice && *(slice++) != ',')
		return (NULL);
	printf("slice before second bitoi: %s", slice);
	if (bitoi(&slice, rgb, 'g') == NULL)
		return (NULL);
	printf("slice after second bitoi: %s", slice);
	if (*slice && *(slice++) != ',')
		return (NULL);
	printf("slice before third bitoi: %s", slice);
	if (bitoi(&slice, rgb, 'b') == NULL)
		return (NULL);
	printf("red value is: %d\n", *rgb >> 16);
	printf("green value is: %d\n", (*rgb >> 8) & 255);
	printf("blue value is: %d\n", *rgb & 255);
	return (slice);
}

char	*get_intensity(char *slice, float *intensity)
{
	slice = skip_space(slice);
	printf("slice is intensity: %s", slice);
	if (ft_atof(&slice, intensity) == NULL)
		return (NULL);
	printf("slice is intensity: %s", slice);
	if (*intensity < 0 || *intensity > 1)
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

int	set_amblight(char *slice, t_scene *scene, char *is_set)
{
	printf("enters set_amblight\n");
	if ((*is_set & 1) == 1)
		 return (0);
	printf("enters set_amblight 2\n");
	slice = get_intensity(slice, &scene->ambient.intensity);
	if (slice == NULL)
		return (0);
	printf("enters set_amblight 3\n");
	slice = get_rgb(slice, &scene->ambient.trgb);
	printf("exits set_amblight 3\n");
	if (slice == NULL)
		return (0);
	printf("enters set_amblight 4\n");
	while (*slice != '\n' && *slice != EOF)
		if (!is_space((*slice)++))
			return (0);
	*is_set += 1;
	return (1);
}

int	set_light(char *slice, t_scene *scene)
{
	static char	is_first;

	if (is_first == 0)
		
	printf("enters set_light\n");
		scene->lights = scene->lights->next;
	slice = get_pos(slice, &scene->lights->pos);
	printf("exits set_light\n");
	return (1);
}

int	set_camera(char *slice, t_scene *scene, char *is_set)
{
	printf("enters set_camera\n");
	if (*is_set >> 1 == 1)
		 return (0);
	slice = get_pos(slice, &scene->camera.pos);
	if (slice == NULL)
		return (0);
	slice = get_dir(slice, &scene->camera.pos);
	if (slice == NULL)
		return (0);
	slice = get_fov(slice, &scene->camera.fov);
	if (slice == NULL)
		return (0);
	*is_set += 2;
	printf("exits set_camera\n");
	return (1);
}

int	set_sphere(char *slice, t_scene *scene)
{
	printf("enters set_sphere\n");
	slice = get_pos(slice, &scene->shapes->pos);
	if (slice == NULL)
		return (0);
	slice = get_parameter(slice, &scene->shapes->figure.sphere.diameter);
	if (slice == NULL)
		return (0);
	printf("exits set_sphere\n");
	return (1);
}

int	set_plane(char *slice, t_scene *scene)
{
	printf("enters set_plane\n");
	slice = get_pos(slice, &scene->shapes->pos);
	if (slice == NULL)
		return (0);
	slice = get_dir(slice, &scene->shapes->figure.plane.dir);
	if (slice == NULL)
		return (0);
	printf("exits set_plane\n");
	return (1);
}

int	set_cylinder(char *slice, t_scene *scene)
{
	printf("enters set_cylinder\n");
	slice = get_pos(slice, &scene->shapes->pos);
	if (slice == NULL)
		return (0);
	slice = get_dir(slice, &scene->shapes->figure.cylinder.dir);
	if (slice == NULL)
		return (0);
	slice = get_parameter(slice, &scene->shapes->figure.cylinder.diameter);
	if (slice == NULL)
		return (0);
	slice = get_parameter(slice, &scene->shapes->figure.cylinder.height);
	if (slice == NULL)
		return (0);
	printf("exits set_cylinder\n");
	return (1);
}

int create_basics(char *slice, t_scene *scene, char * const line)
{
	static char	is_set = 0;

	if (*slice == 'A' && !set_amblight(++slice, scene, &is_set))
	{
		//exit((free(line), 1));
		free(line);
		exit(write(2, "Error\nAmbient light couldn't be set\n", 37));
	}
	else if (*slice == 'L' && !set_light(++slice, scene))
	{
		free(line);
		exit(write(2, "Error\nLight couldn't be set\n", 29));
	}
	else if (*slice == 'C' && !set_camera(++slice, scene, &is_set))
	{
		free(line);
		exit(write(2, "Error\nCamera couldn't be set\n", 30));
	}
	return (1);
}

int create_shape(char *slice, t_scene *scene, char * const line)
{
	if (*slice == 's' && !set_sphere(++slice, scene))
	{
		free(line);
		exit(write(2, "Error\nSphere couldn't be set\n", 30));
	}
	else if (*slice == 'p' && !set_plane(++slice, scene))
	{
		free(line);
		exit(write(2, "Error\nPlane couldn't be set\n", 29));
	}
	else if (*slice == 'c' && !set_cylinder(++slice, scene))
	{
		free(line);
		exit(write(2, "Error\nCylinder couldn't be set\n", 32));
	}
	return (1);
}

void parse_line(char *const line, t_scene *scene)
{
	//WARN: do not modify, slice is used to not tamper with malloc-ed lines
	//		is_set is used as a check for multiple ambient and cameras 
	char		*slice;

	if (!*line)
		exit(write(2, "Error\nWrong format\n", 13));
	slice = skip_space(line);
	if (*slice && *(slice +  1) && is_space(*(slice + 1))
		&& (*slice == 'A' || *slice == 'L' || *slice == 'C'))
	{
		//TODO: create basics
		create_basics(slice, scene, line);
		printf("slice is basic: %s", slice);
		printf("line is basic: %s", line);
	}
	else if ((*slice && *(slice + 1)) && *(slice + 2)
			&& is_space (*(slice + 2)) && (ft_strncmp(slice, "sp", 2) == 0
			|| ft_strncmp(slice, "pl", 2) == 0
			|| ft_strncmp(slice, "cy", 2) == 0))
	{
		//TODO: create shape
		create_shape(slice, scene, line);
		printf("slice is shape: %s", slice);
		printf("line is shape: %s", line);
	}
	else
		//exit(write(2, "Error\nInvalid object\n", 22));
		printf("Invalid shape\n");
}

void minirt_init(t_vars *vars, int fd, t_scene *scene)
{
	char	*line;

	line = get_next_line(fd);
	if (!line)
		exit(write(2, "Error\nEmpty file\n", 18));
	ft_bzero(scene, sizeof(*scene));
	while (line != NULL)
	{
		printf("line is: %s", line);
		parse_line(line, scene);
		free(line);
		line = get_next_line(fd);
	}
	//free(line);
	vars->mlx = mlx_init();
	vars->win = mlx_new_window(vars->mlx, WIDTH, HEIGHT, "miniRT");
}

	//TODO: normalized dir isn't done
	//FIX: malloc and lists (lights and shapes)

int main(int argc, char **argv)
{
	t_vars	vars;
	int		fd;
	t_scene	scene;
	
	if (argc != 2)
		return (write(2, "Error\nIncorrect number of arguments\n", 37));
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return (write(2, "Error\nFile not found\n", 22));
	minirt_init(&vars, fd, &scene);
	key_hooks(&vars);
	mlx_hook(vars.win, DestroyNotify, 0, close_win, &vars);
	mlx_loop(vars.mlx);
	return (0);
}
