"""Python version of the simply SDL Pong. This uses the pygame wrapper
around the SDL bindings: http://pygame.org. Binaries are available from:
http://www.lfd.uci.edu/~gohlke/pythonlibs/#pygame
"""
import pygame
from pygame.locals import *

pygame.init()
pygame.key.set_repeat(5, 5)

background_colour = pygame.Color(0xFF, 0xFF, 0xFF)
background = pygame.display.set_mode((400, 400))
player_colours = [
    background.map_rgb(0xff, 0x00, 0x00),
    background.map_rgb(0x00, 0xff, 0x00),
]
players = [
    pygame.Rect((200, 100), (30, 10)),
    pygame.Rect((200, 300), (30, 10)),
]
ball_colour = background.map_rgb(0xff, 0xff, 0x00)
ball = pygame.Rect((195, 195), (10, 10))
ball_direction = [1, 4]
ball_speed = 1

dPos = 5
movements = {
    "up" : (0, -dPos),
    "down" : (0, +dPos),
    "left" : (-dPos, 0),
    "right" : (+dPos, 0),
}
MOVEMENT_EVENT_TYPE = USEREVENT + 1
Event = pygame.event.Event

quit = False;
while not quit:
    for event in pygame.event.get():
        if event.type == QUIT:
            quit = True
        elif event.type == KEYDOWN:
            if event.key == K_ESCAPE:
                pygame.event.post(pygame.event.Event(QUIT))
            else:
                if event.key == K_UP:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="up", player=1))
                elif event.key == K_DOWN:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="down", player=1))
                elif event.key == K_LEFT:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="left", player=1))
                elif event.key == K_RIGHT:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="right", player=1))
                elif event.key == K_q:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="up", player=2))
                elif event.key == K_a:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="down", player=2))
                elif event.key == K_s:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="left", player=2))
                elif event.key == K_d:
                    pygame.event.post(Event(MOVEMENT_EVENT_TYPE, movement="right", player=2))
        elif event.type == MOVEMENT_EVENT_TYPE:
            player = players[event.player-1]
            dx, dy = movements[event.movement]
            player.left += dx
            player.top += dy


    dx, dy = [ball_speed * d for d in ball_direction]
    ball.left += dx
    ball.top += dy
    n_player = 1 + ball.collidelist(players)

    if n_player != 0:
        ball_direction = [-1 * d for d in ball_direction]
    if not ball.colliderect(background.get_rect()):
        pygame.event.post(Event(QUIT))

    background.fill(background_colour)
    background.fill(ball_colour, ball)
    for player, colour in zip(players, player_colours):
        background.fill(colour, player)
    pygame.display.flip()
    pygame.time.delay(20)

pygame.quit()
