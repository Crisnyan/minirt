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

typedef union surface
{
	float	(*create_sphere)(t_vec3, float);
	float 	(*create_plane)(t_vec3, t_vec3);
	float	(*create_cylinder)(t_vec3, float, float);
}	u_surface;

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
	t_vec3			pos;
	float			intensity;
	int				trgb;
	struct s_light	*next;
}	t_light;

typedef struct s_shape
{
	//TODO:	surface should be a funtion that describes a function, i.e. x^2+y^2=r^2
	t_vec3			pos;
	float			height;	
	float			diameter;	
	u_surface		surface;	
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
	if (digit )
		return (1);
	return (0);
}

char *skip_space(char *const line)
{
	char *slice;

	slice = line;
	while (*slice && is_space(*slice))
		 slice++;
	return (slice);
}

float	ft_atof(char *str)
{
	//FIX: HAY QUE IMPLEMENTAR ATOF???
	
	while (*str != '.')
		if (!is_digit(*str))
	return (0);
}

t_vec3	get_pos(char *slice)
{
	t_vec3	pos;

	slice = skip_space(slice);
	//ft_atof

	return (pos);
}

int	set_amblight(char *slice, t_scene *scene, char *is_set)
{
	printf("enters set_amblight\n");
	if ((*is_set & 1) == 1)
		 return (0);
	*is_set += 1;
	return (1);
}

int	set_light(char *slice, t_scene *scene)
{
	printf("enters set_light\n");
	return (0);
}

int	set_camera(char *slice, t_scene *scene, char *is_set)
{
	printf("enters set_camera\n");
	if (*is_set >> 1 == 1)
		 return (0);
	scene->camera.pos = get_pos(slice);
	*is_set += 2;
	return (1);
}

int	set_sphere(char *slice, t_scene *scene)
{
	printf("enters set_sphere\n");
	return (0);
}

int	set_plane(char *slice, t_scene *scene)
{
	printf("enters set_plane\n");
	return (0);
}

int	set_cylinder(char *slice, t_scene *scene)
{
	printf("enters set_cylinder\n");
	return (0);
}

int create_parameter(char *slice, t_scene *scene, char * const line)
{
	static char	is_set = 0;
	0000 0011

	if (*slice == 'A' && !set_amblight(slice, scene, &is_set))
	{
		free(line);
		exit(write(2, "Error: ambient light couldn't be set\n", 37));
	}
	else if (*slice == 'L' && !set_light(slice, scene))
	{
		free(line);
		exit(write(2, "Error: light couldn't be set\n", 39));
	}
	else if (*slice == 'C' && !set_camera(slice, scene, &is_set))
	{
		free(line);
		exit(write(2, "Error: camera couldn't be set\n", 30));
	}
	return (1);
}

int create_shape(char *slice, t_scene *scene, char * const line)
{
	if (*slice == 's' && !set_sphere(slice, scene))
	{
		free(line);
		exit(write(2, "Error: sphere couldn't be set\n", 30));
	}
	else if (*slice == 'p' && !set_plane(slice, scene))
	{
		free(line);
		exit(write(2, "Error: plane couldn't be set\n", 29));
	}
	else if (*slice == 'c' && !set_cylinder(slice, scene))
	{
		free(line);
		exit(write(2, "Error: cylinder couldn't be set\n", 32));
	}
	return (1);
}

void parse_line(char *const line, t_scene *scene)
{
	//WARN: do not modify, slice is used to not tamper with malloc-ed lines
	//		is_set is used as a check for multiple ambient and cameras 
	char		*slice;

	if (!*line)
		exit(write(2, "Wrong format\n", 13));
	slice = skip_space(line);
	if (*slice == 'A' || *slice == 'L' || *slice == 'C')
	{
		//TODO: create parameter
		create_parameter(slice, scene, line);
		printf("slice is parameter: %s", slice);
		printf("line is shape: %s", line);
	}
	else if ((*slice && *(slice + 1)) && (ft_strncmp(slice, "sp", 2) == 0
		  || ft_strncmp(slice, "pl", 2) == 0 || ft_strncmp(slice, "cy", 2) == 0))
	{
		//TODO: create shape
		create_shape(slice, scene, line);
		printf("slice is shape: %s", slice);
		printf("line is shape: %s", line);
	}
	else
		printf("INVALID SHAPE\n");
}

void minirt_init(t_vars *vars, int fd, t_scene *scene)
{
	char	*line;

	line = get_next_line(fd);
	if (!line)
		exit(write(2, "Empty file\n", 11));
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

int main(int argc, char **argv)
{
	t_vars	vars;
	int		fd;
	t_scene	scene;
	
	if (argc != 2)
		return (write(2, "Incorrect number of arguments\n", 30));
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return (write(2, "File not found\n", 15));
	minirt_init(&vars, fd, &scene);
	key_hooks(&vars);
	mlx_hook(vars.win, DestroyNotify, 0, close_win, &vars);
	mlx_loop(vars.mlx);
	return (0);
}
