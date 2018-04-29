/* some methods related to password reset via emailing a token to user */

async createToken(user: UserDoc, type: Token, expiresIn: string) {
        user.tokenCounter = user.tokenCounter || {}
        user.tokenCounter[type] = user.tokenCounter[type] || 1

        user.markModified('tokenCounter')
        await user.save()

        const token = jwt.sign({
            email: user.email,
            salt: user.salt,
            type,
            tokenCounter: user.tokenCounter[type],
            hashedPassword: user.hashedPassword,
        }, user.salt, {
            expiresIn,
        })

        this.log.info(`new token issued`, {
            user: user.email,
            Token: type,
        })

        return token
}

async verifyToken(token: string, user: UserDoc, type: Token) {
    try {
        const decoded = await promisify(jwt.verify)(token, user.salt)

        if(decoded.email !== user.email) {
            throw new VError({
                decoded,
            }, `invalid token: email`)
        }

        if(decoded.salt !== user.salt) {
            throw new VError({
                decoded,
            }, `invalid token: salt`)
        }

        if(decoded.type !== type) {
            throw new VError({
                decoded,
            }, `invalid token: email`)
        }

        if(decoded.hashedPassword !== user.hashedPassword) {
            throw new VError({
            }, `invalid token: hashedPassword`)
        }

        if(decoded.tokenCounter !== user.tokenCounter[type]) {
            this.log.warn(`user tried to verify invalidated token`, {
                user: user.email,
                tokens: type,
                presentedCounter: decoded.tokenCounter,
                actualCounter: user.tokenCounter[type],
            })
            throw new VError({
                decoded,
            }, `invalid token: counter`)
        }
    } catch(err) {
        return false
    }
    return true
}

async postChangePasswordWithToken(req: Request, res: Response) {
    let user = await this.dbMgr.models.User.findOne({
        email: req.query.email,
    })

    if(!user) {
        return this.errorResult(409, req, res,
            'user not found'
        )
    }

    if(await this.verifyToken(req.query.passwordResetToken, user, 'password reset') ===
        false) {
        return this.errorResult(403, req, res,
            `invalid password reset token`,
        )
    }

    const newPasswordHashed = (await crypto.hash('sha1')(req.query.newPassword + user.salt)).toString('hex')
    user.hashedPassword = newPasswordHashed
    await user.save()

    await this.invalidateTokens(user, `authentication`)
    await this.invalidateTokens(user, `password reset`)

    return this.OKResult(req, res,
        {
            msg: `password changed`,
        }
    )
}

async postResetPassword(req: Request, res: Response) {
    let user = await this.dbMgr.models.User.findOne({
        email: req.query.email,
    })

    if(!user) {
        return this.errorResult(409, req, res,
            'user not found'
        )
    }

    if(!user.emailConfirmed) {
        return this.errorResult(409, req, res,
            'email not confirmed'
        )
    }

    /* we create a 1day valid token
     * old tokens are invalidated
    */
    await this.invalidateTokens(user, 'password reset')
    const passwordResetToken = await this.createToken(user, 'password reset', '1d')

    const url = this.frontRouterFactory.buildFrontURL(
        'changePasswordWithToken', {
            email: user.email,
            passwordResetToken,
        })
    await this.emailSender.sendEmail({
        from: this.fromAddress,
        to: user.email,
        subject: ('CONFIRMATION_EMAIL_SUBJECT'),
        text: ('CONFIRMATION_EMAIL_BODY_PLAIN', url),
        html: ('CONFIRMATION_EMAIL_BODY_HTML', url),
    })

    return this.OKResult(req, res,
        `password resent link sent to the email provided`
    )
}
